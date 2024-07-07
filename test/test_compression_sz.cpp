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
    size_t numElementsToSkip = 3;
    std::vector<vtkm::Int16> data = readDatFile<vtkm::Int16>(file_path, numElementsToSkip);

    vtkm::Id3 dataDimensions(832, 832, 494);
    vtkm::Id3 blockDimensions(256, 256, 256);

    std::vector<vtkm::cont::DataSet> dataSets = splitDataSet(data, dataDimensions, blockDimensions);

    std::vector<vtkm::Float32> isovalues = {50.0f, 100.0f, 150.0f};

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