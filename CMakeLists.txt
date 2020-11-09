cmake_minimum_required(VERSION 3.18)

project(hdf5_cmake_experiment C)

# set(CMAKE_MODULE_PATH "cmake;${CMAKE_MODULE_PATH}")

find_package(HDF5 REQUIRED COMPONENTS C)
# message(${HDF5_INCLUDE_DIRS})
# find_package(HDFEOS5 REQUIRED COMPONENTS C)

add_executable(hdf5_cmake_experiment main.c)
target_include_directories(hdf5_cmake_experiment PRIVATE ${HDF5_INCLUDE_DIRS})
target_link_libraries(hdf5_cmake_experiment PRIVATE ${HDF5_C_LIBRARIES})
