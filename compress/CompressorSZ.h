//
// Created by Yanliang Li on 7/8/24.
//

#ifndef VTK_TRY_COMPRESSORCUSZ_H
#define VTK_TRY_COMPRESSORCUSZ_H

#pragma once
#include <SZ3/api/sz.hpp>
#include <iostream>
#include <vector>


struct CompressionResult {
    std::vector<uint8_t> compressedData;
};

CompressionResult compressDataWithSZ3(const std::vector<float>& data, size_t nx, size_t ny, size_t nz, double errorBound) {

    std::vector<size_t> dims({nx, ny, nz});
    SZ3::Config conf({dims[0], dims[1], dims[2]});
    conf.cmprAlgo = SZ3::ALGO_INTERP_LORENZO;
    conf.errorBoundMode = SZ3::EB_REL;
    conf.relErrorBound = errorBound;

    // Perform compression
    size_t cmpSize;
    char* cmpData = SZ_compress(conf, data.data(), cmpSize);

    std::vector<uint8_t> compressedVec(cmpData, cmpData + cmpSize);

    delete[] cmpData;

    CompressionResult result;
    result.compressedData = compressedVec;
    return result;
}

#endif // VTK_TRY_COMPRESSORCUSZ_H