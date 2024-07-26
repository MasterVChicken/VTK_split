#include "../utils/DataReader.h"
#include "../data_split/DataMerge.h"
#include <vtkm/cont/Initialize.h>
#include <iostream>
#include "../compress/CompressorMGARD_nonCLI.h"
#include <chrono>
#include <cmath> // for std::sqrt
#include <algorithm> // for std::max

int main(int argc, char *argv[]) {
    vtkm::cont::InitializeOptions options =
            vtkm::cont::InitializeOptions::RequireDevice | vtkm::cont::InitializeOptions::AddHelp;
    vtkm::cont::Initialize(argc, argv, options);

    // std::string filePath = "../data/100x500x500/Pf48.bin.f32";
    std::string filePath = "../data/SDRBENCH-EXASKY-NYX-512x512x512/temperature.f32";
    size_t numElements = 512 * 512 * 512;
    std::vector<vtkm::Float32> data = readF32File<vtkm::Float32>(filePath, numElements);

    vtkm::Id3 dataDimensions(500, 500, 100);
    vtkm::Id3 blockDimensions(16, 16, 16);
    int numIsovalues = 5;

    try {
        auto mergedBlocks = findAndMergeIsosurfaceBlocks<vtkm::Float32>(data, dataDimensions, blockDimensions, numIsovalues);

        std::cout << "Merged block positions, sizes, and dimensions: " << std::endl;
        for (const auto &block: mergedBlocks) {
            const auto &mergedData = std::get<0>(block);
            const auto &position = std::get<1>(block);
            const auto &dimensions = std::get<2>(block);
        }

        
        double errorBound = 4e-4;
        std::string s = "infinity"; // Setting s to infinity for L-infinity norm
        double totalCompressionTime = 0;
        double totalDecompressionTime = 0;
        double totalCompressedSize = 0;
        double totalOriginalSize = 0;

        // double totalL2AbsError = 0;
        // double totalL2RelError = 0;
        double totalLinfAbsError = 0;
        double totalLinfRelError = 0;

        for (const auto &block: mergedBlocks) {
            const auto &mergedData = std::get<0>(block);
            const auto &dimensions = std::get<2>(block);

            auto start = std::chrono::high_resolution_clock::now();
            CompressionResult mgardCompressed = compressDataWithMGARDX(mergedData, dimensions[0], dimensions[1], dimensions[2], errorBound, s);
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> compressionTime = end - start;
            totalCompressionTime += compressionTime.count();
            totalCompressedSize += mgardCompressed.compressedData.size();
            totalOriginalSize += mergedData.size() * sizeof(float);

            start = std::chrono::high_resolution_clock::now();
            std::vector<float> decompressedVec = decompressDataWithMGARDX(mgardCompressed.compressedData, dimensions[0], dimensions[1], dimensions[2]);
            end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> decompressionTime = end - start;
            totalDecompressionTime += decompressionTime.count();

            // float l2_abs_error = calculate_L2_error(mergedData, decompressedVec, false);
            // float l2_rel_error = calculate_L2_error(mergedData, decompressedVec, true);
            float linf_abs_error = calculate_Linf_error(mergedData, decompressedVec, false);
            float linf_rel_error = calculate_Linf_error(mergedData, decompressedVec, true);

            // totalL2AbsError += l2_abs_error * l2_abs_error * mergedData.size();
            // totalL2RelError += l2_rel_error * l2_rel_error * mergedData.size();
            totalLinfAbsError = std::max(totalLinfAbsError, static_cast<double>(linf_abs_error));
            totalLinfRelError = std::max(totalLinfRelError, static_cast<double>(linf_rel_error));

            // std::cout << "L2 Absolute Error: " << l2_abs_error << std::endl;
            // std::cout << "L2 Relative Error: " << l2_rel_error << std::endl;
            // std::cout << "L-infinity Absolute Error: " << linf_abs_error << std::endl;
            // std::cout << "L-infinity Relative Error: " << linf_rel_error << std::endl;
        }

        // totalL2AbsError = std::sqrt(totalL2AbsError / totalOriginalSize);
        // totalL2RelError = std::sqrt(totalL2RelError / totalOriginalSize);

        std::cout << "Total Compression Time: " << totalCompressionTime << " seconds" << std::endl;
        std::cout << "Total Decompression Time: " << totalDecompressionTime << " seconds" << std::endl;
        std::cout << "Total Compressed Size: " << totalCompressedSize << " bytes" << std::endl;
        std::cout << "Total Original Size: " << totalOriginalSize << " bytes" << std::endl;
        std::cout << "Compression Ratio: " << totalOriginalSize / totalCompressedSize << std::endl;

        // std::cout << "Total L2 Absolute Error: " << totalL2AbsError << std::endl;
        // std::cout << "Total L2 Relative Error: " << totalL2RelError << std::endl;
        std::cout << "Total L-infinity Absolute Error: " << totalLinfAbsError << std::endl;
        std::cout << "Total L-infinity Relative Error: " << totalLinfRelError << std::endl;

    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}

