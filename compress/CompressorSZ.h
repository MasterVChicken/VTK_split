//
// Created by Yanliang Li on 7/8/24.
//

#ifndef VTK_TRY_COMPRESSORSZ_H
#define VTK_TRY_COMPRESSORSZ_H

#pragma once
#include <vector>
#include <string>
#include <sz.h>

struct CompressionResult {
    std::vector<uint8_t> compressedData;
};

// CompressionResult compressDataWithSZ(const std::vector<float>& data, size_t nx, size_t ny, size_t nz, double errorBound) {
//     size_t outSize;
//     size_t r1 = nx, r2 = ny, r3 = nz;

//     // Initialize SZ with specified error bound
//     sz_params params;
//     memset(&params, 0, sizeof(sz_params)); // Initialize to zero
//     params.absErrBound = errorBound;
//     SZ_Init_Params(&params);

//     // Perform compression
//     unsigned char *compressedData = SZ_compress(SZ_FLOAT, (void*)data.data(), &outSize, 0, 0, r3, r2, r1);

//     // Store the compressed data in a vector
//     std::vector<uint8_t> compressedVec(compressedData, compressedData + outSize);

//     // Clean up
//     free(compressedData);
//     SZ_Finalize();

//     CompressionResult result;
//     result.compressedData = compressedVec;
//     return result;
// }

CompressionResult compressDataWithSZ(const std::vector<float>& data, size_t nx, size_t ny, size_t nz, double errorBound) {
    // Initialize SZ parameters
    sz_params params;
    memset(&params, 0, sizeof(params));
    params.dataType = SZ_FLOAT;
    params.errorBoundMode = REL;
    params.relBoundRatio = errorBound;

    // Initialize SZ library
    SZ_Init(NULL);

    // Allocate output buffer
    size_t outSize = 0;
    unsigned char* compressedData = SZ_compress_args(SZ_FLOAT, (void*)data.data(), &outSize, REL, errorBound, 0, 0, nx, ny, nz);

    if (!compressedData) {
        SZ_Finalize();
        throw std::runtime_error("Compression failed");
    }

    // Copy compressed data to the result
    CompressionResult result;
    result.compressedData.assign(compressedData, compressedData + outSize);

    // Free the allocated memory by SZ
    free(compressedData);

    // Finalize SZ library
    SZ_Finalize();

    return result;
}

#endif //VTK_TRY_COMPRESSORSZ_H
