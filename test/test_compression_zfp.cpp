//
// Created by Yanliang Li on 7/7/24.
//

#include "../data_split/DataSpliter.h"
#include "../utils/DataReader.h"
#include "../extractor/IsosurfaceExtractor.h"
#include <vtkm/cont/Initialize.h>
#include <iostream>
#include <zfp.h>

// Compress with ZFP
std::vector<uint8_t> compressDataWithZFP(const std::vector<float>& data, size_t nx, size_t ny, size_t nz, double tolerance) {
    zfp_type type = zfp_type_float;
    zfp_field* field = zfp_field_3d(const_cast<float*>(data.data()), type, nx, ny, nz);

    // set parameter
    zfp_stream* zfp = zfp_stream_open(nullptr);
    zfp_stream_set_accuracy(zfp, tolerance);

    size_t bufsize = zfp_stream_maximum_size(zfp, field);
    std::vector<uint8_t> buffer(bufsize);

    bitstream* stream = stream_open(buffer.data(), bufsize);
    zfp_stream_set_bit_stream(zfp, stream);
    zfp_stream_rewind(zfp);

    size_t compressedSize = zfp_compress(zfp, field);
    if (compressedSize == 0) {
        std::cerr << "Compression failed!" << std::endl;
    }

    zfp_field_free(field);
    zfp_stream_close(zfp);
    stream_close(stream);

    buffer.resize(compressedSize);
    return buffer;
}

int main(int argc, char* argv[]) {

    double tolerance = 0.01;
    vtkm::cont::InitializeOptions options = vtkm::cont::InitializeOptions::RequireDevice | vtkm::cont::InitializeOptions::AddHelp;
    vtkm::cont::Initialize(argc, argv, options);

    std::string file_path = "../data/stagbeetle832x832x494.dat";
    std::vector<vtkm::Float32> data = readF32File<vtkm::Float32>(file_path);

    vtkm::Id3 dataDimensions(512, 512, 512);
    vtkm::Id3 blockDimensions(64, 64, 64);

    //Get data range and generate meaningful iso-value
    vtkm::Range dataRange = calculateDataRange<vtkm::Float32>(data);

    int numIsovalues = 10;
    std::vector<vtkm::Float32> isovalues = generateIsovalues<vtkm::Float32>(dataRange, numIsovalues);

    std::vector<vtkm::cont::DataSet> dataSets = splitDataSet(data, dataDimensions, blockDimensions);

    std::vector<IsoSurfaceResult> results = processIsovalues(dataSets, isovalues);

    for (const auto& result : results) {
        std::cout << "Isovalue: " << result.isovalue << "\nBlocks: ";
        for (auto blockId : result.blockIds) {
            std::cout << blockId << " ";
        }
        std::cout << std::endl;
    }

    return 0;
}
