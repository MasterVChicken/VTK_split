//
// Created by Yanliang Li on 6/21/24.
//
#include "../utils/DataReader.h"
#include <vtkm/Types.h>
#include <vtkm/cont/DataSet.h>
#include <vtkm/cont/DataSetBuilderUniform.h>
#include <vtkm/cont/ColorTable.h>

#include <vtkm/filter/contour/ContourFlyingEdges.h>

#include <vtkm/rendering/Actor.h>
#include <vtkm/rendering/Scene.h>
#include <vtkm/rendering/CanvasRayTracer.h>
#include <vtkm/rendering/View3D.h>
#include <vtkm/rendering/MapperRayTracer.h>

int main(int argc, char *argv[]) {
    std::string file_path = "../data/stagbeetle832x832x494.dat";
    size_t numElementsToSkip = 3;
    std::vector<vtkm::Int16> data = readDatFile<vtkm::Int16>(file_path, numElementsToSkip);

    // I don't know why it fails when I was using vtkm::Int16 to render the figure directly.
    // But anyway it works with float
    // transform data type to float
    std::vector<float> dataFloat(data.begin(), data.end());

    vtkm::cont::DataSet inputDataSet;
    vtkm::cont::DataSetBuilderUniform dataSetBuilder;

    vtkm::Id3 pointDimensions(832, 832, 494);
    vtkm::Id3 origin(0, 0, 0);
    vtkm::Id3 spacing(1, 1, 1);

    inputDataSet = dataSetBuilder.Create(pointDimensions);
    inputDataSet.AddPointField("data", dataFloat);

    vtkm::filter::contour::ContourFlyingEdges filter;
    vtkm::Float32 isovalue = 100;
    filter.SetIsoValue(isovalue);
    filter.SetActiveField("data");
    vtkm::cont::DataSet outputDataSet = filter.Execute(inputDataSet);

    // Set up a camera for rendering the input data
    vtkm::rendering::Camera camera;
    camera.SetLookAt(vtkm::Vec3f_32(415.5f, 415.5f, 247.0f)); // set look origin at dataset center
    camera.SetViewUp(vtkm::make_Vec(0.f, 1.f, 0.f)); // view straight up
    camera.SetClippingRange(1.f, 1000.f); // set slice range
    camera.SetFieldOfView(60.f); // set view direction
    camera.SetPosition(vtkm::Vec3f_32(415.5f, 415.5f, 1000.0f)); // camera position
    vtkm::cont::ColorTable colorTable("inferno");

    // Background color:
    vtkm::rendering::Color bg(0.2f, 0.2f, 0.2f, 1.0f);
    vtkm::rendering::Actor actor(outputDataSet.GetCellSet(),
                                 outputDataSet.GetCoordinateSystem(),
                                 outputDataSet.GetField("data"));

    vtkm::rendering::Scene scene;
    scene.AddActor(actor);

    vtkm::rendering::CanvasRayTracer canvas(20480,20480);
    vtkm::rendering::View3D view(scene, vtkm::rendering::MapperRayTracer(), canvas, camera, bg);
    view.Paint();
    view.SaveAs("beetle.png");

    return 0;
}

