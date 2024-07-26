#pragma once
#include "mgard/compress_x.hpp"
#include <iostream>
#include <vector>
#include <cstring> // for std::memcpy
#include <cmath> // for std::sqrt, std::abs
#include <string>
#include <limits>
#include <stdexcept>

enum class ErrorType {
    L2,
    Linf
};

// 计算L2误差
float calculate_L2_error(const std::vector<float>& original, const std::vector<float>& decompressed, bool relative) {
    if (original.size() != decompressed.size()) {
        std::cerr << "Error: Data size mismatch! Original size: " << original.size() << ". Decompressed size: " << decompressed.size() << std::endl;
        return -1.0;
    }

    float sum_square_error = 0.0;
    float original_norm = 0.0;
    for (size_t i = 0; i < original.size(); ++i) {
        float diff = original[i] - decompressed[i];
        sum_square_error += diff * diff;
        original_norm += original[i] * original[i];
    }
    float l2_error = std::sqrt(sum_square_error / original.size());
    if (relative) {
        return l2_error / std::sqrt(original_norm / original.size());
    }
    return l2_error;
}

// 计算L-Infinity误差
float calculate_Linf_error(const std::vector<float>& original, const std::vector<float>& decompressed, bool relative) {
    if (original.size() != decompressed.size()) {
        std::cerr << "Error: Data size mismatch! Original size: " << original.size() << ". Decompressed size: " << decompressed.size() << std::endl;
        return -1.0;
    }

    float max_error = 0.0;
    float max_original = 0.0;
    for (size_t i = 0; i < original.size(); ++i) {
        float diff = std::abs(original[i] - decompressed[i]);
        if (diff > max_error) {
            max_error = diff;
        }
        if (std::abs(original[i]) > max_original) {
            max_original = std::abs(original[i]);
        }
    }
    if (relative) {
        return max_error / max_original;
    }
    return max_error;
}

struct CompressionResult {
    std::vector<uint8_t> compressedData;
    mgard_x::compress_status_type status;
};

CompressionResult compressDataWithMGARDX(const std::vector<float>& data, size_t nx, size_t ny, size_t nz, double errorBound, const std::string& s) {
    mgard_x::Config config;
    config.lossless = mgard_x::lossless_type::Huffman;
    config.dev_type = mgard_x::device_type::CUDA;

    std::vector<mgard_x::SIZE> shape{nx, ny, nz};

    float* in_array_cpu = const_cast<float*>(data.data());

    void* compressed_array_cpu = nullptr;
    size_t compressed_size;

    double s_value;
    if (s == "infinity") {
        s_value = std::numeric_limits<double>::infinity();
    } else {
        s_value = std::stod(s); // Convert string to double
    }

    auto status = mgard_x::compress(3, mgard_x::data_type::Float, shape, errorBound, s_value,
                      mgard_x::error_bound_type::REL, in_array_cpu,
                      compressed_array_cpu, compressed_size, config, false);

    if (status != mgard_x::compress_status_type::Success) {
        throw std::runtime_error("Compression failed with status: " + std::to_string(static_cast<int>(status)));
    }

    std::vector<uint8_t> compressedVec(compressed_size);
    std::memcpy(compressedVec.data(), compressed_array_cpu, compressed_size);

    cudaFree(compressed_array_cpu);

    CompressionResult result;
    result.compressedData = compressedVec;
    result.status = status;
    return result;
}

std::vector<float> decompressDataWithMGARDX(const std::vector<uint8_t>& compressedData, size_t nx, size_t ny, size_t nz) {
    mgard_x::Config config;
    config.dev_type = mgard_x::device_type::CUDA;

    std::vector<mgard_x::SIZE> shape{nx, ny, nz};

    void* decompressed_array_cpu = nullptr;

    auto status = mgard_x::decompress(compressedData.data(), compressedData.size(), decompressed_array_cpu, config, false);

    if (status != mgard_x::compress_status_type::Success) {
        throw std::runtime_error("Decompression failed with status: " + std::to_string(static_cast<int>(status)));
    }

    std::vector<float> decompressedVec(nx * ny * nz);
    std::memcpy(decompressedVec.data(), decompressed_array_cpu, nx * ny * nz * sizeof(float));

    cudaFree(decompressed_array_cpu);

    return decompressedVec;
}

