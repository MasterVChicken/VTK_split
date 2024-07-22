// #ifndef VTK_TRY_COMPRESSORZFP_H
// #define VTK_TRY_COMPRESSORZFP_H

#include <vector>
#include <string>
#include <stdexcept>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <algorithm>

// 定义一个压缩结果的结构体
struct CompressionResult {
    std::vector<char> compressedData;
    size_t compressedSize;
    double compressionTime;
};

double computeAbsoluteErrorBound(const std::vector<float>& data, double relativeErrorBound) {
    double maxValue = *std::max_element(data.begin(), data.end());
    return maxValue * relativeErrorBound;
}

CompressionResult compressDataWithZFP(const std::vector<float>& data, int dimX, int dimY, int dimZ, double tolerance) {
    double absoluteErrorBound = computeAbsoluteErrorBound(data, tolerance);
    std::string inputFilePath = "temp_input.bin";
    std::string outputFilePath = "temp_output.bin";

    // 将数据写入文件
    std::ofstream outFile(inputFilePath, std::ios::binary);
    outFile.write(reinterpret_cast<const char*>(data.data()), data.size() * sizeof(float));
    outFile.close();

    // 调用 ZFP 压缩命令
    std::string command = "/home/exouser/zfp/build/bin/zfp -h -f -3 " + std::to_string(dimX) + " " + std::to_string(dimY) + " " + std::to_string(dimZ) + 
                          " -a " + std::to_string(absoluteErrorBound) + " -i " + inputFilePath + " -z " + outputFilePath;
    
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

std::vector<float> decompressDataWithZFP(const std::vector<char>& compressedData, int dimX, int dimY, int dimZ) {
    std::string inputFilePath = "temp_input_compressed.bin";
    std::string outputFilePath = "temp_output_decompressed.bin";

    // 将压缩数据写入文件
    std::ofstream outFile(inputFilePath, std::ios::binary);
    outFile.write(compressedData.data(), compressedData.size());
    outFile.close();

    // 调用 ZFP 解压缩命令
    std::string command = "/home/exouser/zfp/build/bin/zfp -h -z " + inputFilePath + " -o " + outputFilePath;

    int result = std::system(command.c_str());
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


// #endif //VTK_TRY_COMPRESSORZFP_H


