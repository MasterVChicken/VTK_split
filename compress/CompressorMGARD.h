//
// Created by Yanliang Li on 7/8/24.
//

#ifndef VTK_TRY_COMPRESSORMGARD_H
#define VTK_TRY_COMPRESSORMGARD_H

#pragma once
#include "mgard/compress.hpp"
#include "cuda_runtime.h"
#include <iostream>
#include <vector>

struct CompressionResult {
    std::vector<uint8_t> compressedData;
};

CompressionResult compressDataWithMGARDX(const std::vector<float>& data, size_t nx, size_t ny, size_t nz, double errorBound, double s = 0) {
    // Define MGARD configuration
    mgard_x::Config config;
    config.lossless = mgard_x::lossless_type::Huffman_LZ4;
    config.dev_type = mgard_x::device_type::CUDA;

    // Define the shape of the data
    std::vector<mgard_x::SIZE> shape{nx, ny, nz};

    // Calculate the size of the input data
    size_t dataSize = nx * ny * nz * sizeof(float);

    // Prepare input data
    float* in_array_cpu = const_cast<float*>(data.data());

    // Allocate GPU memory
    float* in_array_gpu = nullptr;
    cudaMalloc((void**)&in_array_gpu, dataSize);
    cudaMemcpy(in_array_gpu, in_array_cpu, dataSize, cudaMemcpyDefault);

    // Allocate memory for compressed data
    size_t compressedSize = dataSize + 1e6;  // Estimation
    uint8_t* compressed_array_gpu = nullptr;
    cudaMalloc((void**)&compressed_array_gpu, compressedSize);

    // Compress data using MGARD
    void* compressed_array_gpu_void = static_cast<void*>(compressed_array_gpu);
    mgard_x::compress_status_type status = mgard_x::compress(3, mgard_x::data_type::Float, shape, errorBound, s,
                      mgard_x::error_bound_type::REL, in_array_gpu,
                      compressed_array_gpu_void, compressedSize, config, true);

    if (status != mgard_x::compress_status_type::Success) {
        throw std::runtime_error("MGARD compression failed");
    }

    // Copy compressed data back to CPU
    std::vector<uint8_t> compressedData(compressedSize);
    cudaMemcpy(compressedData.data(), compressed_array_gpu, compressedSize, cudaMemcpyDefault);

    // Free GPU memory
    cudaFree(in_array_gpu);
    cudaFree(compressed_array_gpu);

    // Resize compressed data to actual size
    compressedData.resize(compressedSize);

    CompressionResult result;
    result.compressedData = compressedData;
    return result;
}

#endif //VTK_TRY_COMPRESSORMGARD_H