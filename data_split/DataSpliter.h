//
// Created by Yanliang Li on 6/21/24.
//

#ifndef VTK_TRY_DATASPLIT_H
#define VTK_TRY_DATASPLIT_H

#pragma once
#include <iostream>
#include <stdexcept>

#include <vector>
#include <vtkm/cont/DataSet.h>
#include <vtkm/cont/DataSetBuilderUniform.h>
#include <vtkm/cont/ArrayHandle.h>
#include <vtkm/cont/ArrayCopy.h>
#include <vtkm/cont/Field.h>

template <typename T>
std::vector<vtkm::cont::DataSet> splitDataSet(const std::vector<T>& data,
                                              const vtkm::Id3& dataDimensions,
                                              const vtkm::Id3& blockDimensions) {
    std::vector<vtkm::cont::DataSet> dataSets;
    vtkm::Id3 numBlocks((dataDimensions[0] + blockDimensions[0] - 1) / blockDimensions[0],
                        (dataDimensions[1] + blockDimensions[1] - 1) / blockDimensions[1],
                        (dataDimensions[2] + blockDimensions[2] - 1) / blockDimensions[2]);

    vtkm::cont::DataSetBuilderUniform dataSetBuilder;

    for (vtkm::Id z = 0; z < numBlocks[2]; ++z) {
        for (vtkm::Id y = 0; y < numBlocks[1]; ++y) {
            for (vtkm::Id x = 0; x < numBlocks[0]; ++x) {
                vtkm::Id3 start(x * blockDimensions[0], y * blockDimensions[1], z * blockDimensions[2]);
                vtkm::Id3 end = start + blockDimensions;
                end[0] = std::min(end[0], dataDimensions[0]);
                end[1] = std::min(end[1], dataDimensions[1]);
                end[2] = std::min(end[2], dataDimensions[2]);

                vtkm::Id3 currentBlockDimensions = end - start;
                vtkm::Id blockSize = currentBlockDimensions[0] * currentBlockDimensions[1] * currentBlockDimensions[2];

                std::vector<float> blockData(blockSize);

                for (vtkm::Id bz = 0; bz < currentBlockDimensions[2]; ++bz) {
                    for (vtkm::Id by = 0; by < currentBlockDimensions[1]; ++by) {
                        for (vtkm::Id bx = 0; bx < currentBlockDimensions[0]; ++bx) {
                            vtkm::Id globalIndex = ((start[2] + bz) * dataDimensions[1] + (start[1] + by)) * dataDimensions[0] + (start[0] + bx);
                            vtkm::Id localIndex = (bz * currentBlockDimensions[1] + by) * currentBlockDimensions[0] + bx;
                            blockData[localIndex] = static_cast<float>(data[globalIndex]);
                        }
                    }
                }

                vtkm::cont::DataSet dataSet = dataSetBuilder.Create(currentBlockDimensions);
                dataSet.AddPointField("data", blockData);
                dataSets.push_back(dataSet);
            }
        }
    }

    return dataSets;
}

template <typename T>
vtkm::Range calculateDataRange(const std::vector<T>& data) {
    auto result = std::minmax_element(data.begin(), data.end());
    return vtkm::Range(*result.first, *result.second);
}

template <typename T>
std::vector<T> generateIsovalues(const vtkm::Range& dataRange, int numIsovalues) {
    std::vector<T> isovalues;
    T step = static_cast<T>((dataRange.Max - dataRange.Min) / (numIsovalues + 1));
    for (int i = 1; i <= numIsovalues; ++i) {
        isovalues.push_back(static_cast<T>(dataRange.Min + i * step));
    }
    return isovalues;
}


#endif //VTK_TRY_DATASPLIT_H
