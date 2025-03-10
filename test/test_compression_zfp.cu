#include "../utils/DataReader.h"
#include "../data_split/DataMerge.h"
#include <vtkm/cont/Initialize.h>
#include <iostream>
#include "../compress/CompressorZFP.h"
#include <chrono>
#include <zfp.h>

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
        auto mergedBlocks = findAndMergeIsosurfaceBlocks<vtkm::Float32>(data, dataDimensions, blockDimensions, numIsovalues);

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

            std::cout << "Compressing block at position (" << std::get<1>(block)[0] << ", " << std::get<1>(block)[1] << ", " << std::get<1>(block)[2] << ")" << std::endl;
            std::cout << "Block dimensions: (" << dimensions[0] << ", " << dimensions[1] << ", " << dimensions[2] << ")" << std::endl;
            std::cout << "Merged data size: " << mergedData.size() << std::endl;

            auto start = std::chrono::high_resolution_clock::now();
            CompressionResult zfpCompressed = compressDataWithZFP(mergedData, dimensions[0], dimensions[1], dimensions[2], errorBound);
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> compressionTime = end - start;
            totalCompressionTime += compressionTime.count();
            totalCompressedSize += zfpCompressed.compressedData.size();
            totalOriginalSize += mergedData.size() * sizeof(vtkm::Float32);

            std::cout << "Compression time: " << compressionTime.count() << " seconds" << std::endl;
            std::cout << "Compressed data size: " << zfpCompressed.compressedData.size() << std::endl;

            // Perform decompression
            std::vector<vtkm::Float32> decompressedData(mergedData.size());
            start = std::chrono::high_resolution_clock::now();
            decompressDataWithZFP(zfpCompressed.compressedData.data(), zfpCompressed.compressedData.size(),
                                  decompressedData.data(), dimensions[0], dimensions[1], dimensions[2]);
            end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> decompressionTime = end - start;
            totalDecompressionTime += decompressionTime.count();

            std::cout << "Decompression time: " << decompressionTime.count() << " seconds" << std::endl;
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
