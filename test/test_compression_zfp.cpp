//
// Created by Yanliang Li on 7/7/24.
//

#include "../data_split/DataSpliter.h"
#include "../utils/DataReader.h"
#include "../extractor/IsosurfaceExtractor.h"
#include <vtkm/cont/Initialize.h>
#include <iostream>
#include "../compress/CompressorZFP.h"

int main(int argc, char* argv[]) {

    vtkm::cont::InitializeOptions options = vtkm::cont::InitializeOptions::RequireDevice | vtkm::cont::InitializeOptions::AddHelp;
    vtkm::cont::Initialize(argc, argv, options);

    std::string filePath = "../data/SDRBENCH-EXASKY-NYX-512x512x512/temperature.f32";
    size_t numElements = 512 * 512 * 512;  // 512x512x512 3D 数据
    std::vector<vtkm::Float32> data = readF32File<vtkm::Float32>(filePath, numElements);

    // get data range
    vtkm::Range dataRange = calculateDataRange(data);
    std::cout << "Data range: [" << dataRange.Min << ", " << dataRange.Max << "]" << std::endl;

    // generate 2 iso-values: 1/3 and 2/3
    // std::vector<vtkm::Float32> isovalues = {
    //         static_cast<vtkm::Float32>(dataRange.Min + (dataRange.Max - dataRange.Min) / 3),
    //         static_cast<vtkm::Float32>(dataRange.Min + 2 * (dataRange.Max - dataRange.Min) / 3)
    // };
    std::vector<vtkm::Float32> isovalues = generateIsovalues<vtkm::Float32>(dataRange, 8);

    vtkm::Id3 dataDimensions(512, 512, 512);
    vtkm::Id3 blockDimensions(128, 128, 128);
    std::vector<vtkm::cont::DataSet> dataSets = splitDataSet(data, dataDimensions, blockDimensions);

    std::vector<IsoSurfaceResult> results = processIsovalues(dataSets, isovalues);

    // Perform compression
    double errorBound = 1e-2;
    for (const auto& result : results) {
        for (auto blockId : result.blockIds) {
            auto& dataSet = dataSets[blockId];

            vtkm::cont::ArrayHandle<vtkm::Float32> dataArray;
            vtkm::cont::ArrayCopy(dataSet.GetField("data").GetData().AsArrayHandle<vtkm::cont::ArrayHandle<vtkm::Float32>>(), dataArray);

            std::vector<vtkm::Float32> blockData(dataArray.GetNumberOfValues());
            auto portal = dataArray.ReadPortal();
            for (vtkm::Id i = 0; i < dataArray.GetNumberOfValues(); ++i) {
                blockData[i] = portal.Get(i);
            }

            CompressionResult zfpCompressed = compressDataWithZFP(blockData, blockDimensions[0], blockDimensions[1], blockDimensions[2], errorBound);

            std::cout << "Block ID: " << blockId << ", Isovalue: " << result.isovalue << std::endl;
            std::cout << "ZFP Compressed Size: " << zfpCompressed.compressedData.size() << " bytes" << std::endl;
        }
    }
}
