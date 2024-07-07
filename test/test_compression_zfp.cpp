//
// Created by Yanliang Li on 7/7/24.
//

#include "../data_split/DataSpliter.h"
#include "../utils/DataReader.h"
#include "../extractor/IsosurfaceExtractor.h"
#include <vtkm/cont/Initialize.h>
#include <iostream>

int main(int argc, char* argv[]) {

    double error_bound = 0.01;
    vtkm::cont::InitializeOptions options = vtkm::cont::InitializeOptions::RequireDevice | vtkm::cont::InitializeOptions::AddHelp;
    vtkm::cont::Initialize(argc, argv, options);

    std::string file_path = "../data/stagbeetle832x832x494.dat";
    std::vector<vtkm::Float32> data = readF32File<vtkm::Float32>(file_path);

    vtkm::Id3 dataDimensions(512, 512, 512);
    vtkm::Id3 blockDimensions(64, 64, 64);

    //Get data range and generate meaningful iso-value
    vtkm::Range dataRange = calculateDataRange<vtkm::Float32>(data);

    int numIsovalues = 10;
    std::vector<vtkm::Float32> isovalues = generateIsovalues<vtkm::Float32>(dataRange, numIsovalues);

    std::vector<vtkm::cont::DataSet> dataSets = splitDataSet(data, dataDimensions, blockDimensions);

    std::vector<IsoSurfaceResult> results = processIsovalues(dataSets, isovalues);

    for (const auto& result : results) {
        std::cout << "Isovalue: " << result.isovalue << "\nBlocks: ";
        for (auto blockId : result.blockIds) {
            std::cout << blockId << " ";
        }
        std::cout << std::endl;
    }

    return 0;
}
