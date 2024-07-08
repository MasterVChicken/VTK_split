//
// Created by Yanliang Li on 7/7/24.
//

#ifndef VTK_TRY_COMPRESSOR_H
#define VTK_TRY_COMPRESSOR_H

#pragma once
#include <vector>
#include <string>
#include <zfp.h>

struct CompressionResult {
    std::vector<uint8_t> compressedData;
};

CompressionResult compressDataWithZFP(const std::vector<float>& data, size_t nx, size_t ny, size_t nz, double tolerance) {
    zfp_type type = zfp_type_float;
    zfp_field* field = zfp_field_3d(const_cast<float*>(data.data()), type, nx, ny, nz);
    zfp_stream* zfp = zfp_stream_open(nullptr);
    zfp_stream_set_accuracy(zfp, tolerance);
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

#endif //VTK_TRY_COMPRESSOR_H
