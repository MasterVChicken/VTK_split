//
// Created by Yanliang Li on 6/21/24.
//

#ifndef VTK_TRY_DATASPLIT_H
#define VTK_TRY_DATASPLIT_H

#include <iostream>
#include <vector>
#include <stdexcept>

// recurisve slice
template <typename T>
void sliceRecursive(const T* data, const std::vector<size_t>& dims, const std::vector<std::pair<size_t, size_t>>& ranges,
                    const std::vector<size_t>& strides, size_t dim, size_t originalIndex, size_t slicedIndex, std::vector<T>& slicedData) {
    if (dim == dims.size()) {
        slicedData[slicedIndex] = data[originalIndex];
        return;
    }
    for (size_t i = ranges[dim].first; i < ranges[dim].second; ++i) {
        sliceRecursive(data, dims, ranges, strides, dim + 1, originalIndex + i * strides[dim],
                       slicedIndex + (i - ranges[dim].first) * strides[dim], slicedData);
    }
}

template <typename T>
std::vector<T> sliceData(const T* data, const std::vector<size_t>& dims, const std::vector<std::pair<size_t, size_t>>& ranges) {
    // check whether range and data dimensions pair
    if (dims.size() != ranges.size()) {
        throw std::invalid_argument("Dimension size and range size must be equal");
    }

    // calculate size after slice
    std::vector<size_t> slicedDims(dims.size());
    size_t sliceSize = 1;
    for (size_t i = 0; i < dims.size(); ++i) {
        size_t start = ranges[i].first;
        size_t end = ranges[i].second;
        if (start >= dims[i] || end > dims[i] || start >= end) {
            throw std::out_of_range("Invalid range specified for slicing");
        }
        slicedDims[i] = end - start;
        sliceSize *= slicedDims[i];
    }

    // compute original stride before slice
    std::vector<size_t> strides(dims.size(), 1);
    for (size_t i = dims.size() - 1; i > 0; --i) {
        strides[i - 1] = strides[i] * dims[i];
    }

    // allocate space
    std::vector<T> slicedData(sliceSize);

    // call recursive slice function
    sliceRecursive(data, dims, ranges, strides, 0, 0, 0, slicedData);

    return slicedData;
}



#endif //VTK_TRY_DATASPLIT_H
