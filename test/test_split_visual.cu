#include <vtkm/filter/contour/ContourFlyingEdges.h>
#include <vtkm/cont/Initialize.h>
#include <vtkm/cont/DataSet.h>
#include <vtkm/cont/DataSetBuilderUniform.h>
#include <chrono>
#include "../utils/DataReader.h"
#include "../data_split/IsosurfaceExtractor.h"
#include "../data_split/DataSpliter.h"

int main(int argc, char *argv[]){
    vtkm::cont::InitializeOptions options =
            vtkm::cont::InitializeOptions::RequireDevice | vtkm::cont::InitializeOptions::AddHelp;
    vtkm::cont::Initialize(argc, argv, options);
    vtkm::Id3 blockDimensions(128, 128, 128);

    std::string filePath1 = "../data/100x500x500/Pf48.bin.f32";
    size_t numElements1 = 500 * 500 * 100;   
    std::vector<vtkm::Float32> data1 = readF32File<vtkm::Float32>(filePath1, numElements1);
    vtkm::Id3 dataDimensions1(500, 500, 100);
    

    std::string filePath2 = "../data/SDRBENCH-EXASKY-NYX-512x512x512/temperature.f32";
    size_t numElements2 = 512 * 512 * 512;   
    std::vector<vtkm::Float32> data2 = readF32File<vtkm::Float32>(filePath2, numElements2);
    vtkm::Id3 dataDimensions2(512, 512, 512);
    

    std::string filePath3 = "../data/SDRBENCH-SCALE_98x1200x1200/QS-98x1200x1200.f32";
    size_t numElements3 = 1200 * 1200 * 98;   
    std::vector<vtkm::Float32> data3 = readF32File<vtkm::Float32>(filePath3, numElements3);
    vtkm::Id3 dataDimensions3(1200, 1200, 98);

    int numIsovalues = 5;
    
    // auto blocksWithPosition1 = findIsosurfaceBlocks<float>(data1, dataDimensions1, blockDimensions, numIsovalues);
    auto blocksWithPosition2 = findIsosurfaceBlocks<float>(data2, dataDimensions2, blockDimensions, numIsovalues);
    // auto blocksWithPosition3 = findIsosurfaceBlocks<float>(data3, dataDimensions3, blockDimensions, numIsovalues);
        
    return 0;
}