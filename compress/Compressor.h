//
// Created by Yanliang Li on 7/7/24.
//

#ifndef VTK_TRY_COMPRESSOR_H
#define VTK_TRY_COMPRESSOR_H

#pragma once
#include <vector>
#include <string>
#include <zfp.h>
#include <sz.h>
#include <mgard/compress_x.hpp>

struct CompressionResult {
    std::vector<uint8_t> compressedData;
};

CompressionResult compressDataWithZFP(const std::vector<float>& data, size_t nx, size_t ny, size_t nz, double errorBound) {
    zfp_type type = zfp_type_float;
    zfp_field* field = zfp_field_3d(const_cast<float*>(data.data()), type, nx, ny, nz);
    zfp_stream* zfp = zfp_stream_open(nullptr);
    zfp_stream_set_accuracy(zfp, errorBound);
    size_t bufsize = zfp_stream_maximum_size(zfp, field);
    std::vector<uint8_t> buffer(bufsize);
    bitstream* stream = stream_open(buffer.data(), bufsize);
    zfp_stream_set_bit_stream(zfp, stream);
    zfp_stream_rewind(zfp);
    size_t compressedSize = zfp_compress(zfp, field);
    if (compressedSize == 0) {
        throw std::runtime_error("Compression failed");
    }
    zfp_field_free(field);
    zfp_stream_close(zfp);
    stream_close(stream);
    buffer.resize(compressedSize);

    CompressionResult result;
    result.compressedData = buffer;
    return result;
}

CompressionResult compressDataWithSZ(const std::vector<float>& data, size_t nx, size_t ny, size_t nz, double errorBound) {
    size_t outSize;
    size_t r1 = nx, r2 = ny, r3 = nz;

    // Initialize SZ with specified error bound
    SZ_Init(NULL);
    sz_params *params = SZ_getParams();
    params->absErrBound = errorBound;

    // Perform compression
    unsigned char *compressedData = SZ_compress(SZ_FLOAT, data.data(), &outSize, r3, r2, r1);

    // Store the compressed data in a vector
    std::vector<uint8_t> compressedVec(compressedData, compressedData + outSize);

    // Clean up
    free(compressedData);
    SZ_Finalize();

    CompressionResult result;
    result.compressedData = compressedVec;
    return result;
}

CompressionResult compressDataWithMGARDx(const std::vector<float>& data, size_t nx, size_t ny, size_t nz, double errorBound) {
    // Define data shape
    std::vector<mgard_x::SIZE> shape = {nz, ny, nx};

    // Prepare input and output arrays
    float *in_array_cpu = new float[data.size()];
    std::copy(data.begin(), data.end(), in_array_cpu);

    void *compressed_array_cpu = NULL;
    size_t compressed_size;
    double s = 0;

    // Set up MGARD-X configuration
    mgard_x::Config config;
    config.lossless = mgard_x::lossless_type::Huffman_Zstd;
    config.dev_type = mgard_x::device_type::CUDA;

    // Perform compression
    mgard_x::compress(3, mgard_x::data_type::Float, shape, errorBound, s,
                      mgard_x::error_bound_type::REL, in_array_cpu,
                      compressed_array_cpu, compressed_size, config, false);

    // Store the compressed data in a vector
    std::vector<uint8_t> compressedVec(compressed_size);
    std::memcpy(compressedVec.data(), compressed_array_cpu, compressed_size);

    // Clean up
    delete[] in_array_cpu;
    mgard_x::free(compressed_array_cpu);

    CompressionResult result;
    result.compressedData = compressedVec;
    return result;
}

#endif //VTK_TRY_COMPRESSOR_H
