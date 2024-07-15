cmake_minimum_required(VERSION 3.21)
project(VTK_split LANGUAGES CXX CUDA)

# 添加包含路径
include_directories(/usr/local/include /home/exouser/zfp/include /home/exouser/MGARD/install-cuda-ampere/include)

# 添加库路径
link_directories(/usr/local/lib /home/exouser/zfp/build/lib /home/exouser/MGARD/install-cuda-ampere/lib)

# 查找并链接ZFP库
#find_library(ZFP_LIB zfp PATHS /home/exouser/zfp/build/lib)

# 查找并链接SZ库
#set(CUSZ_DIR "/home/exouser/sz-install/lib/cmake/CUSZ")
#set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${CUSZ_DIR})
#find_package(CUSZ REQUIRED)
#
#find_library(PSZ_LIB NAMES psz_cu_utils PATHS /home/exouser/sz-install/lib)

# for zstd in sz3
link_directories(/home/exouser/MGARD/install-cuda-ampere/lib)
include_directories(/home/exouser/MGARD/install-cuda-ampere/include)

set(SZ3_ROOT /home/exouser/sz3-install)
include_directories(${SZ3_ROOT}/include)
link_directories(${SZ3_ROOT}/lib)
find_library(SZ3_LIB NAMES SZ3c PATHS ${SZ3_ROOT}/lib)

# 查找并链接Protobuf
#set(Protobuf_INCLUDE_DIR "/home/exouser/MGARD/install-cuda-ampere/include")
#set(Protobuf_LIBRARIES "/home/exouser/MGARD/install-cuda-ampere/lib/libprotobuf.so")
#find_package(Protobuf REQUIRED)
#include_directories(${Protobuf_INCLUDE_DIR})

# 查找并链接MGARD库
#set(MGARD_DIR "/home/exouser/MGARD/install-cuda-ampere/lib/cmake")
#set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${MGARD_DIR})
#find_package(MGARD REQUIRED)

# 手动指定MGARD库路径
#find_library(MGARD_LIB NAMES mgard PATHS /home/exouser/MGARD/install-cuda-ampere/lib)

# 查找并链接其他必要的库
set(VTKm_DIR "/home/exouser/vtk-m-v2.1.0/build/lib/cmake/vtkm-2.1")

find_package(VTKm REQUIRED)
find_package(CUDA REQUIRED)


# 包含依赖项路径
include_directories(${VTKm_INCLUDE_DIRS})
include_directories(${CUDA_INCLUDE_DIRS})
include_directories(/usr/include)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CUDA_STANDARD 11)
set(CMAKE_CUDA_STANDARD_REQUIRED ON)

enable_language(CUDA)

set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -arch=sm_60")

# 添加可执行文件
#cuda_add_executable(testCompressionZFP test/test_compression_zfp.cu)
#set_target_properties(testCompressionZFP PROPERTIES CUDA_SEPARABLE_COMPILATION ON)
#set_property(TARGET testCompressionZFP PROPERTY CUDA_ARCHITECTURES 80)
#target_link_libraries(testCompressionZFP vtkm::filter vtkm::cont vtkm::rendering vtkm::io vtkm::source ${CUDA_LIBRARIES} ${ZFP_LIB})

cuda_add_executable(testCompressionSZ test/test_compression_sz.cu)
set_target_properties(testCompressionSZ PROPERTIES CUDA_SEPARABLE_COMPILATION ON)
set_property(TARGET testCompressionSZ PROPERTY CUDA_ARCHITECTURES 80)
target_link_libraries(testCompressionSZ vtkm::filter vtkm::cont vtkm::rendering vtkm::io vtkm::source ${CUDA_LIBRARIES} ${SZ3_LIB} zstd)

#cuda_add_executable(testCompressionMGARD test/test_compression_mgard.cu)
#set_target_properties(testCompressionMGARD PROPERTIES CUDA_SEPARABLE_COMPILATION ON)
#set_property(TARGET testCompressionMGARD PROPERTY CUDA_ARCHITECTURES 80)
#target_link_libraries(testCompressionMGARD vtkm::filter vtkm::cont vtkm::rendering vtkm::io vtkm::source ${CUDA_LIBRARIES} ${MGARD_LIB} mgard::mgard)