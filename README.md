# VTK_split
## Software environments
* CMake: 3.21+
* CUDA: 12.2
* NVIDIA Driver Version: 535.129.03

How to install dependencies?
```console
$ wget https://gitlab.kitware.com/vtk/vtk-m/-/archive/v2.1.0/vtk-m-v2.1.0.tar.gz
$ tar xvzf vtk-m-v2.1.0.tar.gz
$ cd vtkm-m-v2.1.0
$ mkdir build
$ cd build
$ cmake .. -DVTKm_ENABLE_CUDA:BOOL=ON -DCMAKE_CUDA_COMPILER=/usr/local/cuda-12.2/bin/nvcc
$ make -j4
$ sudo make install
$ sudo ldconfig
```
Then vtk-m with CUDA support is installed successfully.
How to run? The testing dataset for now is 'stagbeetle832x832x494.dat'。

```console
$ cd build
$ cmake ..
$ make -j4
$ ./testSplit --vtkm-device Cuda
```
Then the ouput png file will be in /build directory.