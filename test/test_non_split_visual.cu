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

    std::string filePath1 = "../data/100x500x500/Pf48.bin.f32";
    size_t numElements1 = 500 * 500 * 100;   
    std::vector<vtkm::Float32> data1 = readF32File<vtkm::Float32>(filePath1, numElements1);

    std::string filePath2 = "../data/SDRBENCH-EXASKY-NYX-512x512x512/temperature.f32";
    size_t numElements2 = 512 * 512 * 512;   
    std::vector<vtkm::Float32> data2 = readF32File<vtkm::Float32>(filePath2, numElements2);

    std::string filePath3 = "../data/SDRBENCH-SCALE_98x1200x1200/QS-98x1200x1200.f32";
    size_t numElements3 = 1200 * 1200 * 98;   
    std::vector<vtkm::Float32> data3 = readF32File<vtkm::Float32>(filePath3, numElements3);

    int numIsovalues = 5;
    
    auto dataRange1 = calculateDataRange(data1);
    auto isovalues1 = generateIsovalues<float>(dataRange1, numIsovalues);

    auto dataRange2 = calculateDataRange(data2);
    auto isovalues2 = generateIsovalues<float>(dataRange2, numIsovalues);

    auto dataRange3 = calculateDataRange(data3);
    auto isovalues3 = generateIsovalues<float>(dataRange3, numIsovalues);

    vtkm::cont::DataSetBuilderUniform dataSetBuilder;
    vtkm::Id3 blockDims1(500,500,100);
    vtkm::cont::DataSet dataSet1 = dataSetBuilder.Create(blockDims1);
    dataSet1.AddPointField("data", data1);
    std::vector<vtkm::cont::DataSet> dataSets1;
    dataSets1.push_back(dataSet1);

    vtkm::Id3 blockDims2(512,512,512);
    vtkm::cont::DataSet dataSet2 = dataSetBuilder.Create(blockDims2);
    dataSet2.AddPointField("data", data2);
    std::vector<vtkm::cont::DataSet> dataSets2;
    dataSets2.push_back(dataSet2); 

    vtkm::Id3 blockDims3(1200,1200,98);
    vtkm::cont::DataSet dataSet3 = dataSetBuilder.Create(blockDims3);
    dataSet3.AddPointField("data", data3);
    std::vector<vtkm::cont::DataSet> dataSets3;
    dataSets3.push_back(dataSet3);

    auto isoSurfaceResults1 = processIsovalues(dataSets1, isovalues1);
    auto isoSurfaceResults2 = processIsovalues(dataSets2, isovalues2);
    auto isoSurfaceResults3 = processIsovalues(dataSets3, isovalues3);
        
    return 0;
}