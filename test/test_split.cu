//
// Created by Yanliang Li on 7/1/24.
//
#include "../data_split/DataSpliter.h"
#include "../utils/DataReader.h"
#include <vtkm/Types.h>
#include <vtkm/cont/ColorTable.h>

#include <vtkm/filter/contour/ContourFlyingEdges.h>

#include <vtkm/rendering/Actor.h>
#include <vtkm/rendering/Scene.h>
#include <vtkm/rendering/CanvasRayTracer.h>
#include <vtkm/rendering/View3D.h>
#include <vtkm/rendering/MapperRayTracer.h>

#include <vtkm/cont/cuda/DeviceAdapterCuda.h>
#include <vtkm/cont/Initialize.h>

void renderDataSet(const vtkm::cont::DataSet& dataSet, const std::string& outputFileName) {
    vtkm::filter::contour::ContourFlyingEdges filter;
    vtkm::Float32 isovalue = 100;
    filter.SetIsoValue(isovalue);
    filter.SetActiveField("data");
    vtkm::cont::DataSet outputDataSet = filter.Execute(dataSet);

    // Set up a camera for rendering the input data
    vtkm::rendering::Camera camera;
    camera.SetLookAt(vtkm::Vec3f_32(256, 256, 256));
    camera.SetViewUp(vtkm::make_Vec(0.f, 1.f, 0.f));
    camera.SetClippingRange(1.f, 1000.f);
    camera.SetFieldOfView(60.f);
    camera.SetPosition(vtkm::Vec3f_32(1.5, 1.5, 1.5));
    vtkm::cont::ColorTable colorTable("inferno");

    // Background color:
    vtkm::rendering::Color bg(0.2f, 0.2f, 0.2f, 1.0f);
    vtkm::rendering::Actor actor(outputDataSet.GetCellSet(),
                                 outputDataSet.GetCoordinateSystem(),
                                 outputDataSet.GetField("data"));

    vtkm::rendering::Scene scene;
    scene.AddActor(actor);

    vtkm::rendering::CanvasRayTracer canvas(2048, 2048);
    vtkm::rendering::View3D view(scene, vtkm::rendering::MapperRayTracer(), canvas, camera, bg);
    view.Paint();
    view.SaveAs(outputFileName);
}

int main(int argc, char *argv[]) {
    vtkm::cont::InitializeOptions options = vtkm::cont::InitializeOptions::RequireDevice | vtkm::cont::InitializeOptions::AddHelp;
    vtkm::cont::Initialize(argc, argv, options);

    std::string file_path = "../data/stagbeetle832x832x494.dat";
    size_t numElementsToSkip = 3;
    std::vector<vtkm::Int16> data = readDatFile<vtkm::Int16>(file_path, numElementsToSkip);

    vtkm::Id3 dataDimensions(832, 832, 494);
    vtkm::Id3 blockDimensions(256, 256, 256);

    std::vector<vtkm::cont::DataSet> dataSets = splitDataSet(data, dataDimensions, blockDimensions);

    for (size_t i = 0; i < dataSets.size(); ++i) {
        std::string outputFileName = "beetle_block_" + std::to_string(i) + ".png";
        renderDataSet(dataSets[i], outputFileName);
    }

    return 0;
}