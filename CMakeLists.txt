cmake_minimum_required(VERSION 3.21)
project(VTK_split LANGUAGES CXX CUDA)

find_package(VTKm REQUIRED)
find_package(CUDA REQUIRED)

include_directories(${VTKm_INCLUDE_DIRS})
include_directories(${CUDA_INCLUDE_DIRS})

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CUDA_STANDARD 11)
set(CMAKE_CUDA_STANDARD_REQUIRED ON)

enable_language(CUDA)

set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -arch=sm_60")

set(SOURCES main.cpp)

# cuda_add_executable(VTK_try ${SOURCES})
# set_target_properties(VTK_try PROPERTIES CUDA_SEPARABLE_COMPILATION ON)
# target_link_libraries(VTK_try vtkm::filter vtkm::cont vtkm::rendering vtkm::io vtkm::source ${CUDA_LIBRARIES})

# cuda_add_executable(testVisualization test/test_visualization.cpp)
# set_target_properties(testVisualization PROPERTIES CUDA_SEPARABLE_COMPILATION ON)
# target_link_libraries(testVisualization vtkm::filter vtkm::cont vtkm::rendering vtkm::io vtkm::source ${CUDA_LIBRARIES})

# cuda_add_executable(testResolution test/test_resolution.cpp)
# set_target_properties(testResolution PROPERTIES CUDA_SEPARABLE_COMPILATION ON)
# target_link_libraries(testResolution vtkm::filter vtkm::cont vtkm::rendering vtkm::io vtkm::source ${CUDA_LIBRARIES})

cuda_add_executable(testSplit test/test_split.cu)
set_target_properties(testSplit PROPERTIES CUDA_SEPARABLE_COMPILATION ON)
set_property(TARGET testSplit PROPERTY CUDA_ARCHITECTURES 80)
target_link_libraries(testSplit vtkm::filter vtkm::cont vtkm::rendering vtkm::io vtkm::source ${CUDA_LIBRARIES})