//
// Created by Yanliang Li on 7/7/24.
//

#include "../data_split/DataSpliter.h"
#include "../utils/DataReader.h"
#include "../extractor/IsosurfaceExtractor.h"
#include <vtkm/cont/Initialize.h>
#include <iostream>
#include "../compress/Compressor.h"

int main(int argc, char* argv[]) {

    vtkm::cont::InitializeOptions options = vtkm::cont::InitializeOptions::RequireDevice | vtkm::cont::InitializeOptions::AddHelp;
    vtkm::cont::Initialize(argc, argv, options);

    std::string filePath = "../data/stagbeetle832x832x494.dat";
    size_t numElements = 512 * 512 * 512;  // 512x512x512 3D 数据
    std::vector<vtkm::Float32> data = readF32File(filePath, numElements);

    // get data range
    vtkm::Range dataRange = calculateDataRange(data);
    std::cout << "Data range: [" << dataRange.Min << ", " << dataRange.Max << "]" << std::endl;

    // generate 2 iso-values: 1/3 and 2/3
    std::vector<vtkm::Float32> isovalues = {
            static_cast<vtkm::Float32>(dataRange.Min + (dataRange.Max - dataRange.Min) / 3),
            static_cast<vtkm::Float32>(dataRange.Min + 2 * (dataRange.Max - dataRange.Min) / 3)
    };

    vtkm::Id3 dataDimensions(512, 512, 512);
    vtkm::Id3 blockDimensions(128, 128, 128);
    std::vector<vtkm::cont::DataSet> dataSets = splitDataSet(data, dataDimensions, blockDimensions);

    std::vector<IsoSurfaceResult> results = processIsovalues(dataSets, isovalues);

    // Perform compression
    double errorBound = 1e-2;
    for (const auto& result : results) {
        for (auto blockId : result.blockIds) {
            auto& dataSet = dataSets[blockId];
            std::vector<vtkm::Float32> blockData;
            dataSet.GetField("data").GetData().CopyTo(blockData);

            CompressionResult zfpCompressed = compressDataWithZFP(blockData, blockDimensions[0], blockDimensions[1], blockDimensions[2], errorBound);

            std::cout << "Block ID: " << blockId << ", Isovalue: " << result.isovalue << std::endl;
            std::cout << "ZFP Compressed Size: " << zfpCompressed.compressedData.size() << " bytes" << std::endl;
        }
    }
}
