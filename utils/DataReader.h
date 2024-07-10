//
// Created by Yanliang Li on 6/29/24.
//

#ifndef VTK_TRY_DATAREADER_H
#define VTK_TRY_DATAREADER_H

#include <fstream>
#include <vector>
#include <stdexcept>

// Data reader designed for .dat file to skip metadata
template <typename T>
std::vector<T> readDatFile(const std::string& filename, size_t numElementsToSkip = 3) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Unable to open file: " + filename);
    }

    // get file size
    file.seekg(0, std::ios::end);
    std::size_t fileSize = file.tellg();

    // skip some metadata, for the dataset 'stagbeetle832x832x494.dat', the metadata size is 3
    size_t headerSize = numElementsToSkip * sizeof(T);

    file.seekg(headerSize, std::ios::beg);

    std::size_t dataSize = (fileSize - headerSize) / sizeof(T);
    std::vector<T> data(dataSize);

    // read data
    file.read(reinterpret_cast<char*>(data.data()), dataSize * sizeof(T));

    if (!file) {
        throw std::runtime_error("Error reading file: " + filename);
    }

    file.close();
    return data;
}

// Without header
template <typename T>
std::vector<T> readF32File(const std::string& filename, std::size_t numElements) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Unable to open file: " + filename);
    }

    // get file size
    file.seekg(0, std::ios::end);
    std::size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // calculate the number of elements to read
    std::size_t dataSize = std::min(fileSize / sizeof(T), numElements);
    std::vector<T> data(dataSize);

    // read data
    file.read(reinterpret_cast<char*>(data.data()), dataSize * sizeof(T));

    if (!file) {
        throw std::runtime_error("Error reading file: " + filename);
    }

    return data;
}

#endif //VTK_TRY_DATAREADER_H
