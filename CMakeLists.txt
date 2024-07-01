cmake_minimum_required(VERSION 3.27)
project(VTK_try)
find_package(VTKm REQUIRED)

set(CMAKE_CXX_STANDARD 11)

#add_executable(VTK_try main.cpp)
#target_link_libraries(VTK_try vtkm::filter vtkm::cont vtkm::rendering vtkm::io vtkm::source)

add_executable(testSplit test/test_split.cpp)
target_link_libraries(testSplit vtkm::filter vtkm::cont vtkm::rendering vtkm::io vtkm::source)

#add_executable(testResolution test/test_resolution.cpp)
#target_link_libraries(testResolution vtkm::filter vtkm::cont vtkm::rendering vtkm::io vtkm::source)