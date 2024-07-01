cmake_minimum_required(VERSION 3.27)
project(VTK_split)

find_package(VTKm REQUIRED)
find_package(CUDA REQUIRED)

include_directories(${VTKm_INCLUDE_DIRS})
include_directories(${CUDA_INCLUDE_DIRS})

vtkm_option(VTKm_ENABLE_CUDA "Enable Cuda support" OFF)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CUDA_STANDARD 11)
set(CMAKE_CUDA_STANDARD_REQUIRED ON)

set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -arch=sm_60")

set(SOURCES main.cpp)

add_executable(VTK_try ${SOURCES})
set_target_properties(VTK_try PROPERTIES CUDA_SEPARABLE_COMPILATION ON)
target_link_libraries(VTK_try vtkm::filter vtkm::cont vtkm::rendering vtkm::io vtkm::source ${CUDA_LIBRARIES})

add_executable(testVisualization test/test_visualization.cpp)
set_target_properties(testVisualization PROPERTIES CUDA_SEPARABLE_COMPILATION ON)
target_link_libraries(testVisualization vtkm::filter vtkm::cont vtkm::rendering vtkm::io vtkm::source ${CUDA_LIBRARIES})

add_executable(testResolution test/test_resolution.cpp)
set_target_properties(testResolution PROPERTIES CUDA_SEPARABLE_COMPILATION ON)
target_link_libraries(testResolution vtkm::filter vtkm::cont vtkm::rendering vtkm::io vtkm::source ${CUDA_LIBRARIES})

add_executable(testSplit test/test_split.cpp)
set_target_properties(testSplit PROPERTIES CUDA_SEPARABLE_COMPILATION ON)
target_link_libraries(testSplit vtkm::filter vtkm::cont vtkm::rendering vtkm::io vtkm::source ${CUDA_LIBRARIES})