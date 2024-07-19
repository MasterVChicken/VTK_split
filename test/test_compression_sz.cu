#include "../utils/DataReader.h"
#include "../data_split/DataMerge.h"
#include <vtkm/cont/Initialize.h>
#include <iostream>
#include "../compress/CompressorSZ.h"

int main(int argc, char* argv[]) {

    vtkm::cont::InitializeOptions options = vtkm::cont::InitializeOptions::RequireDevice | vtkm::cont::InitializeOptions::AddHelp;
    vtkm::cont::Initialize(argc, argv, options);

    std::string filePath = "../data/SDRBENCH-EXASKY-NYX-512x512x512/temperature.f32";
    size_t numElements = 512 * 512 * 512;  // 512x512x512 3D 数据
    std::vector<vtkm::Float32> data = readF32File<vtkm::Float32>(filePath, numElements);

    vtkm::Id3 dataDimensions(512, 512, 512);
    vtkm::Id3 blockDimensions(32, 32, 32);
    int numIsovalues = 5;

    try {
        auto mergedBlocks = findAndMergeIsosurfaceBlocks(data, dataDimensions, blockDimensions, numIsovalues);

        std::cout << "Merged block positions, sizes, and dimensions: " << std::endl;
        for (const auto& block : mergedBlocks) {
            const auto& mergedData = std::get<0>(block);
            const auto& position = std::get<1>(block);
            const auto& dimensions = std::get<2>(block);
            std::cout << "Position: (" << position[0] << ", " << position[1] << ", " << position[2] << "), Size: " << mergedData.size() << ", Dimensions: (" << dimensions[0] << ", " << dimensions[1] << ", " << dimensions[2] << ")" << std::endl;
        }

        // Perform compression
        double errorBound = 1e-3;
        for (const auto& block : mergedBlocks) {
            const auto& mergedData = std::get<0>(block);
            const auto& position = std::get<1>(block);
            const auto& dimensions = std::get<2>(block);

            std::cout << "Compressing block at position: (" << position[0] << ", " << position[1] << ", " << position[2] << "), Size: " << mergedData.size() << ", Dimensions: (" << dimensions[0] << ", " << dimensions[1] << ", " << dimensions[2] << ")" << std::endl;
            CompressionResult szCompressed = compressDataWithSZ3(mergedData, dimensions[0], dimensions[1], dimensions[2], errorBound);

            std::cout << "SZ Compressed Size: " << szCompressed.compressedData.size() << " bytes" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
