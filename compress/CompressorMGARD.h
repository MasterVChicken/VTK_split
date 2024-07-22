//
// Created by Yanliang Li on 7/8/24.
//

#ifndef VTK_TRY_COMPRESSORMGARD_H
#define VTK_TRY_COMPRESSORMGARD_H

// #pragma once
// #include "mgard/compress_x.hpp"
// #include <iostream>
// #include <vector>
// #include <cstring> // for std::memcpy
// #include "cuda_runtime.h"

// struct CompressionResult {
//     std::vector<uint8_t> compressedData;
// };

// CompressionResult compressDataWithMGARDX(const std::vector<float>& data, size_t nx, size_t ny, size_t nz, double errorBound, double s = 0) {
//     // Define MGARD configuration
//     mgard_x::Config config;
//     config.lossless = mgard_x::lossless_type::Huffman;
//     config.dev_type = mgard_x::device_type::SERIAL;

//     std::vector<mgard_x::SIZE> shape{nx, ny, nz};

//     // Prepare input data
//     float* in_array_cpu = const_cast<float*>(data.data());

//     // Prepare output data
//     void* compressed_array_cpu = nullptr;
//     size_t compressed_size;

//     // Perform compression
//     mgard_x::compress(3, mgard_x::data_type::Float, shape, errorBound, s,
//                       mgard_x::error_bound_type::REL, in_array_cpu,
//                       compressed_array_cpu, compressed_size, config, false);

//     // Copy compressed data to std::vector
//     std::vector<uint8_t> compressedVec(compressed_size);
//     std::memcpy(compressedVec.data(), compressed_array_cpu, compressed_size);

//     // Clean up
//     cudaFree(compressed_array_cpu);

//     CompressionResult result;
//     result.compressedData = compressedVec;
//     return result;
// }

#ifndef VTK_TRY_COMPRESSORMGARDX_H
#define VTK_TRY_COMPRESSORMGARDX_H

#include <vector>
#include <string>
#include <stdexcept>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <fstream>

// 定义一个压缩结果的结构体
struct CompressionResult {
    std::vector<char> compressedData;
    size_t compressedSize;
    double compressionTime;
};

CompressionResult compressDataWithMGARDX(const std::vector<float>& data, int dimX, int dimY, int dimZ, double relativeErrorBound) {
    std::string inputFilePath = "temp_input_mgardx.bin";
    std::string outputFilePath = "temp_output_mgardx.bin";

    // 将数据写入文件
    std::ofstream outFile(inputFilePath, std::ios::binary);
    outFile.write(reinterpret_cast<const char*>(data.data()), data.size() * sizeof(float));
    outFile.close();

    // 调用 MGARD-X 压缩命令
    std::string command = "/home/exouser/MGARD/install-cuda-ampere/bin/mgard-x -z -i " + inputFilePath + " -c " + outputFilePath + " -t s -n 3 " 
                        + std::to_string(dimX) + " " + std::to_string(dimY) + " " + std::to_string(dimZ) 
                        + " -m rel -e " + std::to_string(relativeErrorBound)+ " -s 0 -r 1 -d serial -l 0";
    
    auto start = std::chrono::high_resolution_clock::now();
    int result = std::system(command.c_str());
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> compressionTime = end - start;

    if (result != 0) {
        throw std::runtime_error("Compression failed");
    }

    // 读取压缩后的数据
    std::ifstream inFile(outputFilePath, std::ios::binary | std::ios::ate);
    std::streamsize size = inFile.tellg();
    inFile.seekg(0, std::ios::beg);

    std::vector<char> compressedData(size);
    inFile.read(compressedData.data(), size);
    inFile.close();

    return {compressedData, static_cast<size_t>(size), compressionTime.count()};
}

std::vector<float> decompressDataWithMGARDX(const std::vector<char>& compressedData, int dimX, int dimY, int dimZ) {
    std::string inputFilePath = "temp_input_compressed_mgardx.bin";
    std::string outputFilePath = "temp_output_decompressed_mgardx.bin";

    // 将压缩数据写入文件
    std::ofstream outFile(inputFilePath, std::ios::binary);
    outFile.write(compressedData.data(), compressedData.size());
    outFile.close();

    // 调用 MGARD-X 解压缩命令
    std::string command = "/home/exouser/MGARD/install-cuda-ampere/bin/mgard-x -x -c " + inputFilePath + " -o " + outputFilePath + " -d serial";

    auto start = std::chrono::high_resolution_clock::now();
    int result = std::system(command.c_str());
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> decompressionTime = end - start;

    if (result != 0) {
        throw std::runtime_error("Decompression failed");
    }

    // 读取解压后的数据
    std::ifstream inFile(outputFilePath, std::ios::binary | std::ios::ate);
    std::streamsize size = inFile.tellg();
    inFile.seekg(0, std::ios::beg);

    std::vector<float> decompressedData(size / sizeof(float));
    inFile.read(reinterpret_cast<char*>(decompressedData.data()), size);
    inFile.close();

    return decompressedData;
}

#endif //VTK_TRY_COMPRESSORMGARDX_H


#endif // VTK_TRY_COMPRESSORMGARD_H