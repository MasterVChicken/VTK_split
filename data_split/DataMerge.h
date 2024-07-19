#ifndef VTK_TRY_DATAMERGE_H
#define VTK_TRY_DATAMERGE_H

#pragma once
#include <set>
#include <stack>
#include <vtkm/cont/DataSet.h>
#include "DataSpliter.h"
#include <iostream> // for debug output
#include <vector>
#include <tuple>
#include <algorithm>

template <typename T>
int maxHist(int row[], int C, int &startIdx, int &endIdx)
{
    std::stack<int> result;
    int top_val;
    int max_area = 0;
    int area = 0;
    int i = 0;
    startIdx = endIdx = -1;
    while (i < C) {
        if (result.empty() || row[result.top()] <= row[i])
            result.push(i++);
        else {
            top_val = row[result.top()];
            result.pop();
            area = top_val * i;
            if (!result.empty())
                area = top_val * (i - result.top() - 1);
            if (area > max_area) {
                max_area = area;
                startIdx = result.empty() ? 0 : result.top() + 1;
                endIdx = i - 1;
            }
        }
    }
    while (!result.empty()) {
        top_val = row[result.top()];
        result.pop();
        area = top_val * i;
        if (!result.empty())
            area = top_val * (i - result.top() - 1);
        if (area > max_area) {
            max_area = area;
            startIdx = result.empty() ? 0 : result.top() + 1;
            endIdx = i - 1;
        }
    }
    return max_area;
}

template <typename T>
int maxCuboid(std::vector<std::vector<std::vector<int>>>& matrix, int R, int C, int D, std::tuple<int, int, int>& start, std::tuple<int, int, int>& end)
{
    int max_volume = 0;

    std::vector<std::vector<int>> hist(R, std::vector<int>(C, 0));

    for (int z = 0; z < D; ++z) {
        for (int x = 0; x < R; ++x) {
            for (int y = 0; y < C; ++y) {
                if (matrix[x][y][z] == 0) {
                    hist[x][y] = 0;
                } else {
                    hist[x][y] += 1;
                }
            }
        }

        for (int i = 0; i < R; ++i) {
            int startIdx = 0, endIdx = 0;
            int area = maxHist<T>(hist[i].data(), C, startIdx, endIdx);
            int volume = area * (z + 1);
            if (volume > max_volume) {
                max_volume = volume;
                start = std::make_tuple(i, startIdx, 0);
                end = std::make_tuple(i, endIdx, z);
            }
        }
    }

    return max_volume;
}

template <typename T>
std::vector<std::tuple<std::vector<T>, vtkm::Id3, vtkm::Id3>> mergeIsosurfaceBlocks(
    const std::vector<T>& data,
    const std::vector<std::tuple<std::vector<T>, vtkm::Id3>>& blocksWithPosition,
    const vtkm::Id3& dataDimensions, const vtkm::Id3& blockDimensions)
{
    if (blockDimensions[0] == 0 || blockDimensions[1] == 0 || blockDimensions[2] == 0) {
        throw std::invalid_argument("blockDimensions must be non-zero");
    }

    vtkm::Id3 numBlocks(
        (dataDimensions[0] + blockDimensions[0] - 1) / blockDimensions[0],
        (dataDimensions[1] + blockDimensions[1] - 1) / blockDimensions[1],
        (dataDimensions[2] + blockDimensions[2] - 1) / blockDimensions[2]);

    std::vector<std::vector<std::vector<int>>> matrix(
        numBlocks[0],
        std::vector<std::vector<int>>(numBlocks[1], std::vector<int>(numBlocks[2], 0)));

    for (const auto& blockWithPosition : blocksWithPosition) {
        const auto& position = std::get<1>(blockWithPosition);
        vtkm::Id x = position[0] / blockDimensions[0];
        vtkm::Id y = position[1] / blockDimensions[1];
        vtkm::Id z = position[2] / blockDimensions[2];

        if (x < 0 || x >= numBlocks[0] || y < 0 || y >= numBlocks[1] || z < 0 || z >= numBlocks[2]) {
            std::cerr << "Error: position out of bounds (" << position[0] << ", " << position[1] << ", " << position[2] << ")" << std::endl;
            continue;
        }

        matrix[x][y][z] = 1;
    }

    std::vector<std::tuple<std::vector<T>, vtkm::Id3, vtkm::Id3>> mergedBlocks;

    while (true) {
        std::tuple<int, int, int> start, end;
        int max_volume = maxCuboid<T>(matrix, numBlocks[0], numBlocks[1], numBlocks[2], start, end);

        if (max_volume == 0) {
            break;
        }

        auto [sx, sy, sz] = start;
        auto [ex, ey, ez] = end;

        std::vector<T> mergedData;
        vtkm::Id3 mergePosition = {sx * blockDimensions[0], sy * blockDimensions[1], sz * blockDimensions[2]};
        vtkm::Id3 mergedDimensions = {(ex - sx + 1) * blockDimensions[0], (ey - sy + 1) * blockDimensions[1], (ez - sz + 1) * blockDimensions[2]};

        for (int x = sx; x <= ex; ++x) {
            for (int y = sy; y <= ey; ++y) {
                for (int z = sz; z <= ez; ++z) {
                    for (int bx = 0; bx < blockDimensions[0]; ++bx) {
                        for (int by = 0; by < blockDimensions[1]; ++by) {
                            for (int bz = 0; bz < blockDimensions[2]; ++bz) {
                                int global_x = x * blockDimensions[0] + bx;
                                int global_y = y * blockDimensions[1] + by;
                                int global_z = z * blockDimensions[2] + bz;
                                if (global_x < dataDimensions[0] && global_y < dataDimensions[1] && global_z < dataDimensions[2]) {
                                    vtkm::Id globalIndex = (global_z * dataDimensions[1] + global_y) * dataDimensions[0] + global_x;
                                    mergedData.push_back(data[globalIndex]);
                                }
                            }
                        }
                    }
                    matrix[x][y][z] = 0;
                }
            }
        }

        mergedBlocks.emplace_back(mergedData, mergePosition, mergedDimensions);
    }

    return mergedBlocks;
}

template <typename T>
std::vector<std::tuple<std::vector<T>, vtkm::Id3, vtkm::Id3>> findAndMergeIsosurfaceBlocks(const std::vector<T>& data,
                                                                                           const vtkm::Id3& dataDimensions,
                                                                                           const vtkm::Id3& blockDimensions,
                                                                                           int numIsovalues)
{
    auto blocksWithPosition = findIsosurfaceBlocks<T>(data, dataDimensions, blockDimensions, numIsovalues);
    return mergeIsosurfaceBlocks<T>(data, blocksWithPosition, dataDimensions, blockDimensions);
}

#endif //VTK_TRY_DATAMERGE_H
