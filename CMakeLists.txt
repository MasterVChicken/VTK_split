cmake_minimum_required(VERSION 3.21)
project(VTK_split LANGUAGES CXX CUDA)

find_package(VTKm REQUIRED)
find_package(CUDA REQUIRED)
find_package(ZFP REQUIRED)
find_package(SZ REQUIRED)
find_package(MGARD REQUIRED)

include_directories(${VTKm_INCLUDE_DIRS})
include_directories(${CUDA_INCLUDE_DIRS})
include_directories(${ZFP_INCLUDE_DIRS})
include_directories(${SZ_INCLUDE_DIRS})
include_directories(${MGARD_INCLUDE_DIRS})

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CUDA_STANDARD 11)
set(CMAKE_CUDA_STANDARD_REQUIRED ON)

enable_language(CUDA)

set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -arch=sm_60")

#set(SOURCES main.cpp)

# cuda_add_executable(VTK_try ${SOURCES})
# set_target_properties(VTK_try PROPERTIES CUDA_SEPARABLE_COMPILATION ON)
# target_link_libraries(VTK_try vtkm::filter vtkm::cont vtkm::rendering vtkm::io vtkm::source ${CUDA_LIBRARIES})

# cuda_add_executable(testVisualization test/test_visualization.cpp)
# set_target_properties(testVisualization PROPERTIES CUDA_SEPARABLE_COMPILATION ON)
# target_link_libraries(testVisualization vtkm::filter vtkm::cont vtkm::rendering vtkm::io vtkm::source ${CUDA_LIBRARIES})

# cuda_add_executable(testResolution test/test_resolution.cpp)
# set_target_properties(testResolution PROPERTIES CUDA_SEPARABLE_COMPILATION ON)
# target_link_libraries(testResolution vtkm::filter vtkm::cont vtkm::rendering vtkm::io vtkm::source ${CUDA_LIBRARIES})

#cuda_add_executable(testSplit test/test_split.cu)
#set_target_properties(testSplit PROPERTIES CUDA_SEPARABLE_COMPILATION ON)
#set_property(TARGET testSplit PROPERTY CUDA_ARCHITECTURES 80)
#target_link_libraries(testSplit vtkm::filter vtkm::cont vtkm::rendering vtkm::io vtkm::source ${CUDA_LIBRARIES})

#cuda_add_executable(testExtractor test/test_extractor.cu)
#set_target_properties(testExtractor PROPERTIES CUDA_SEPARABLE_COMPILATION ON)
#set_property(TARGET testExtractor PROPERTY CUDA_ARCHITECTURES 80)
#target_link_libraries(testExtractor vtkm::filter vtkm::cont vtkm::rendering vtkm::io vtkm::source ${CUDA_LIBRARIES})

cuda_add_executable(testCompressionZFP test/test_compression_zfp.cpp)
set_target_properties(testCompressionZFP PROPERTIES CUDA_SEPARABLE_COMPILATION ON)
set_property(TARGET testCompressionZFP PROPERTY CUDA_ARCHITECTURES 80)
target_link_libraries(testCompressionZFP vtkm::filter vtkm::cont vtkm::rendering vtkm::io vtkm::source ${CUDA_LIBRARIES} ${ZFP_LIBRARIES})

cuda_add_executable(testCompressionSZ test/test_compression_sz.cpp)
set_target_properties(testCompressionSZ PROPERTIES CUDA_SEPARABLE_COMPILATION ON)
set_property(TARGET testCompressionSZ PROPERTY CUDA_ARCHITECTURES 80)
target_link_libraries(testCompressionSZ vtkm::filter vtkm::cont vtkm::rendering vtkm::io vtkm::source ${CUDA_LIBRARIES} ${SZ_LIBRARIES})

cuda_add_executable(testCompressionMGARD test/test_compression_mgard.cpp)
set_target_properties(testCompressionMGARD PROPERTIES CUDA_SEPARABLE_COMPILATION ON)
set_property(TARGET testCompressionMGARD PROPERTY CUDA_ARCHITECTURES 80)
target_link_libraries(testCompressionMGARD vtkm::filter vtkm::cont vtkm::rendering vtkm::io vtkm::source ${CUDA_LIBRARIES} ${MGARD_LIBRARIES})