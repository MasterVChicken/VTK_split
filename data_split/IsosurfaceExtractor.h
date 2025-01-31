//
// Created by Yanliang Li on 7/2/24.
//

#ifndef VTK_TRY_ISOSURFACEEXTRACTOR_H
#define VTK_TRY_ISOSURFACEEXTRACTOR_H

#pragma once
#include <vtkm/filter/contour/ContourFlyingEdges.h>
#include <vtkm/cont/DataSet.h>
#include <vtkm/cont/DataSetBuilderUniform.h>
#include <chrono>

struct IsoSurfaceResult {
    vtkm::Float32 isovalue;
    std::vector<size_t> blockIds;
};

std::vector<IsoSurfaceResult>
processIsovalues(const std::vector<vtkm::cont::DataSet> &dataSets, const std::vector<vtkm::Float32> &isovalues) {
    std::vector<IsoSurfaceResult> results;
    auto start = std::chrono::high_resolution_clock::now();

    for (auto isovalue: isovalues) {
        IsoSurfaceResult result;
        result.isovalue = isovalue;

        for (size_t i = 0; i < dataSets.size(); ++i) {
            vtkm::filter::contour::ContourFlyingEdges filter;
            filter.SetIsoValue(isovalue);
            filter.SetActiveField("data");
            auto outputDataSet = filter.Execute(dataSets[i]);

            if (outputDataSet.GetCellSet().GetNumberOfCells() > 0) {
                result.blockIds.push_back(i);
            }
        }

        results.push_back(result);
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> totalTime = end - start;
    std::cout << "Total Isosurface Extraction Time: " << totalTime.count() << " seconds" << std::endl;

    return results;
}

#endif //VTK_TRY_ISOSURFACEEXTRACTOR_H
