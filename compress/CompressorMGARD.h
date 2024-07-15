//
// Created by Yanliang Li on 7/8/24.
//

#ifndef VTK_TRY_COMPRESSORMGARD_H
#define VTK_TRY_COMPRESSORMGARD_H

#pragma once
#include "mgard/compress_x.hpp"
#include <iostream>
#include <vector>
#include <cstring> // for std::memcpy
#include "cuda_runtime.h"

struct CompressionResult {
    std::vector<uint8_t> compressedData;
};

CompressionResult compressDataWithMGARDX(const std::vector<float>& data, size_t nx, size_t ny, size_t nz, double errorBound, double s = 0) {
    // Define MGARD configuration
    mgard_x::Config config;
    config.lossless = mgard_x::lossless_type::Huffman;
    config.dev_type = mgard_x::device_type::SERIAL;

    std::vector<mgard_x::SIZE> shape{nx, ny, nz};

    // Prepare input data
    float* in_array_cpu = const_cast<float*>(data.data());

    // Prepare output data
    void* compressed_array_cpu = nullptr;
    size_t compressed_size;

    // Perform compression
    mgard_x::compress(3, mgard_x::data_type::Float, shape, errorBound, s,
                      mgard_x::error_bound_type::REL, in_array_cpu,
                      compressed_array_cpu, compressed_size, config, false);

    // Copy compressed data to std::vector
    std::vector<uint8_t> compressedVec(compressed_size);
    std::memcpy(compressedVec.data(), compressed_array_cpu, compressed_size);

    // Clean up
    cudaFree(compressed_array_cpu);

    CompressionResult result;
    result.compressedData = compressedVec;
    return result;
}

#endif // VTK_TRY_COMPRESSORMGARD_H