//
// Created by Yanliang Li on 7/7/24.
//

#include "../utils/DataReader.h"
#include "../data_split/DataMerge.h"
#include <vtkm/cont/Initialize.h>
#include <iostream>
#include "../compress/CompressorSZ.h"
#include <chrono>

int main(int argc, char *argv[]) {

    vtkm::cont::InitializeOptions options =
            vtkm::cont::InitializeOptions::RequireDevice | vtkm::cont::InitializeOptions::AddHelp;
    vtkm::cont::Initialize(argc, argv, options);

    std::string filePath = "../data/SDRBENCH-SCALE_98x1200x1200/QS-98x1200x1200.f32";
    size_t numElements = 1200 * 1200 * 98;   
    std::vector<vtkm::Float32> data = readF32File<vtkm::Float32>(filePath, numElements);

    vtkm::Id3 dataDimensions(1200, 1200, 98);
    vtkm::Id3 blockDimensions(32, 32, 32);
    int numIsovalues = 5;

    try {
        auto mergedBlocks =
                findAndMergeIsosurfaceBlocks < vtkm::Float32 > (data, dataDimensions, blockDimensions, numIsovalues);

        std::cout << "Merged block positions, sizes, and dimensions: " << std::endl;
        for (const auto &block: mergedBlocks) {
            const auto &mergedData = std::get<0>(block);
            const auto &position = std::get<1>(block);
            const auto &dimensions = std::get<2>(block);
            std::cout << "Position: (" << position[0] << ", " << position[1] << ", " << position[2] << "), Size: "
                      << mergedData.size() << ", Dimensions: (" << dimensions[0] << ", " << dimensions[1] << ", "
                      << dimensions[2] << ")" << std::endl;
        }

        // Perform compression
        double errorBound = 1e-2;
        double totalCompressionTime = 0;
        double totalDecompressionTime = 0;
        double totalCompressedSize = 0;
        double totalOriginalSize = 0;

        for (const auto &block: mergedBlocks) {
            const auto &mergedData = std::get<0>(block);
            const auto &dimensions = std::get<2>(block);

            auto start = std::chrono::high_resolution_clock::now();
            CompressionResult szCompressed = compressDataWithSZ3< vtkm::Float32 >(mergedData, dimensions[0], dimensions[1],
                                                                 dimensions[2], errorBound);
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> compressionTime = end - start;
            totalCompressionTime += compressionTime.count();
            totalCompressedSize += szCompressed.compressedData.size();
            totalOriginalSize += mergedData.size() * 4;

            // Ensure decompressedData is correctly allocated
            std::vector<vtkm::Float32> decompressedData(mergedData.size());
            vtkm::Float32* decompressedDataPtr = decompressedData.data();

            SZ3::Config conf;
            start = std::chrono::high_resolution_clock::now();
            SZ_decompress(conf, reinterpret_cast<char*>(szCompressed.compressedData.data()), szCompressed.compressedData.size(), decompressedDataPtr);
            end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> decompressionTime = end - start;
            totalDecompressionTime += decompressionTime.count();

        }
        std::cout << "Total Compression Time: " << totalCompressionTime << " seconds" << std::endl;
        std::cout << "Total Decompression Time: " << totalDecompressionTime << " seconds" << std::endl;
        std::cout << "Total Compressed Size: " << totalCompressedSize << " bytes" << std::endl;
        std::cout << "Total Original Size: " << totalOriginalSize << " bytes" << std::endl;
        std::cout << "Compression Ratio: " << totalOriginalSize / totalCompressedSize << std::endl;
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}