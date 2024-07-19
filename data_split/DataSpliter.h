//
// Created by Yanliang Li on 6/21/24.
//

#ifndef VTK_TRY_DATASPLIT_H
#define VTK_TRY_DATASPLIT_H

#pragma once
#include <iostream>
#include <stdexcept>

#include <vector>
#include <stack>
#include <vtkm/cont/DataSet.h>
#include <vtkm/cont/DataSetBuilderUniform.h>
#include <vtkm/cont/ArrayHandle.h>
#include <vtkm/cont/ArrayCopy.h>
#include <vtkm/cont/Field.h>
#include "IsosurfaceExtractor.h"

//template <typename T>
//std::vector<vtkm::cont::DataSet> splitDataSet(const std::vector<T>& data,
//                                              const vtkm::Id3& dataDimensions,
//                                              const vtkm::Id3& blockDimensions) {
//    std::vector<vtkm::cont::DataSet> dataSets;
//    vtkm::Id3 numBlocks((dataDimensions[0] + blockDimensions[0] - 1) / blockDimensions[0],
//                        (dataDimensions[1] + blockDimensions[1] - 1) / blockDimensions[1],
//                        (dataDimensions[2] + blockDimensions[2] - 1) / blockDimensions[2]);
//
//    vtkm::cont::DataSetBuilderUniform dataSetBuilder;
//
//    for (vtkm::Id z = 0; z < numBlocks[2]; ++z) {
//        for (vtkm::Id y = 0; y < numBlocks[1]; ++y) {
//            for (vtkm::Id x = 0; x < numBlocks[0]; ++x) {
//                vtkm::Id3 start(x * blockDimensions[0], y * blockDimensions[1], z * blockDimensions[2]);
//                vtkm::Id3 end = start + blockDimensions;
//                end[0] = std::min(end[0], dataDimensions[0]);
//                end[1] = std::min(end[1], dataDimensions[1]);
//                end[2] = std::min(end[2], dataDimensions[2]);
//
//                vtkm::Id3 currentBlockDimensions = end - start;
//                vtkm::Id blockSize = currentBlockDimensions[0] * currentBlockDimensions[1] * currentBlockDimensions[2];
//
//                std::vector<float> blockData(blockSize);
//
//                for (vtkm::Id bz = 0; bz < currentBlockDimensions[2]; ++bz) {
//                    for (vtkm::Id by = 0; by < currentBlockDimensions[1]; ++by) {
//                        for (vtkm::Id bx = 0; bx < currentBlockDimensions[0]; ++bx) {
//                            vtkm::Id globalIndex = ((start[2] + bz) * dataDimensions[1] + (start[1] + by)) * dataDimensions[0] + (start[0] + bx);
//                            vtkm::Id localIndex = (bz * currentBlockDimensions[1] + by) * currentBlockDimensions[0] + bx;
//                            blockData[localIndex] = static_cast<float>(data[globalIndex]);
//                        }
//                    }
//                }
//
//                vtkm::cont::DataSet dataSet = dataSetBuilder.Create(currentBlockDimensions);
//                dataSet.AddPointField("data", blockData);
//                dataSets.push_back(dataSet);
//            }
//        }
//    }
//
//    return dataSets;
//}

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

template <typename T>
bool containsIsosurface(const std::vector<T>& blockData, const std::vector<T>& isovalues) {
    for (const T& value : blockData) {
        for (const T& isovalue : isovalues) {
            if (value == isovalue) {
                return true;
            }
        }
    }
    return false;
}

// For this function, the return value should be a vector of tuples which contain data and blockID(int vtkm::Id3 format)
template <typename T>
std::vector<std::tuple<std::vector<T>, vtkm::Id3>> splitDataSet(const std::vector<T>& data,
                                                                const vtkm::Id3& dataDimensions,
                                                                const vtkm::Id3& blockDimensions) {
    std::vector<std::tuple<std::vector<T>, vtkm::Id3>> blocks;
    vtkm::Id3 numBlocks((dataDimensions[0] + blockDimensions[0] - 1) / blockDimensions[0],
                        (dataDimensions[1] + blockDimensions[1] - 1) / blockDimensions[1],
                        (dataDimensions[2] + blockDimensions[2] - 1) / blockDimensions[2]);

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

                std::vector<T> blockData(blockSize);
                for (vtkm::Id bz = 0; bz < currentBlockDimensions[2]; ++bz) {
                    for (vtkm::Id by = 0; by < currentBlockDimensions[1]; ++by) {
                        for (vtkm::Id bx = 0; bx < currentBlockDimensions[0]; ++bx) {
                            vtkm::Id globalIndex = ((start[2] + bz) * dataDimensions[1] + (start[1] + by)) * dataDimensions[0] + (start[0] + bx);
                            vtkm::Id localIndex = (bz * currentBlockDimensions[1] + by) * currentBlockDimensions[0] + bx;
                            blockData[localIndex] = static_cast<T>(data[globalIndex]);
                        }
                    }
                }

                blocks.emplace_back(blockData, start);
            }
        }
    }

    return blocks;
}


template <typename T>
std::vector<std::tuple<std::vector<T>, vtkm::Id3>> findIsosurfaceBlocks(const std::vector<T>& data,
                                                                        const vtkm::Id3& dataDimensions,
                                                                        const vtkm::Id3& blockDimensions,
                                                                        int numIsovalues) {
    auto blocksWithPosition = splitDataSet(data, dataDimensions, blockDimensions);
    std::vector<vtkm::cont::DataSet> dataSets;

    for (const auto& blockWithPosition : blocksWithPosition) {
        // get data
        const auto& blockData = std::get<0>(blockWithPosition);
        vtkm::cont::DataSetBuilderUniform dataSetBuilder;
        vtkm::Id3 blockDims(std::min(blockDimensions[0], dataDimensions[0] - std::get<1>(blockWithPosition)[0]),
                            std::min(blockDimensions[1], dataDimensions[1] - std::get<1>(blockWithPosition)[1]),
                            std::min(blockDimensions[2], dataDimensions[2] - std::get<1>(blockWithPosition)[2]));
        vtkm::cont::DataSet dataSet = dataSetBuilder.Create(blockDims);
        dataSet.AddPointField("data", blockData);
        dataSets.push_back(dataSet);
    }

    auto dataRange = calculateDataRange(data);
    auto isovalues = generateIsovalues<T>(dataRange, numIsovalues);
    auto isoSurfaceResults = processIsovalues(dataSets, isovalues);

    // Use set to make sure no repeated elements in the set
    std::set<size_t> blockIdsWithIsosurface;
    for (const auto& result : isoSurfaceResults) {
        blockIdsWithIsosurface.insert(result.blockIds.begin(), result.blockIds.end());
    }

    std::vector<std::tuple<std::vector<T>, vtkm::Id3>> blocksContainingIsosurface;
    for (size_t i = 0; i < blocksWithPosition.size(); ++i) {
        if (blockIdsWithIsosurface.find(i) != blockIdsWithIsosurface.end()) {
            blocksContainingIsosurface.push_back(blocksWithPosition[i]);
        }
    }

    return blocksContainingIsosurface;
}


#endif //VTK_TRY_DATASPLIT_H
