cmake_minimum_required(VERSION 3.18)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(hdf5_cmake_experiment C)

# _P

# set(CMAKE_MODULE_PATH "cmake;${CMAKE_MODULE_PATH}")

message("CMAKE_PREFIX_PATH: ${CMAKE_PREFIX_PATH}")
# message("Python3_ROOT_DIR: ${Python3_ROOT_DIR}")
# message("CMAKE_MODULE_PATH: ${CMAKE_MODULE_PATH}")

find_package(HDF5 REQUIRED COMPONENTS C HL)
message("HDF5_INCLUDE_DIRS: ${HDF5_INCLUDE_DIRS}")
message("HDF5_C_LIBRARIES: ${HDF5_C_LIBRARIES}")
message("HDF5_C_HL_LIBRARIES: ${HDF5_C_HL_LIBRARIES}")

add_executable(hdf5_cmake_experiment hdf5_cmake_experiment.c)
target_include_directories(hdf5_cmake_experiment PRIVATE ${HDF5_INCLUDE_DIRS})
target_link_libraries(hdf5_cmake_experiment PRIVATE ${HDF5_C_LIBRARIES})

# find_package(he5_hdfeos REQUIRED COMPONENTS C)
set(HDFEOS5_INCLUDE_DIRS "${CMAKE_PREFIX_PATH}/include")
set(HDFEOS5_C_LIBRARIES "${CMAKE_PREFIX_PATH}/lib/libhe5_hdfeos.a;${CMAKE_PREFIX_PATH}/lib/libGctp.a")
message("HDFEOS5_INCLUDE_DIRS: ${HDFEOS5_INCLUDE_DIRS}")
message("HDFEOS5_C_LIBRARIES: ${HDFEOS5_C_LIBRARIES}")

find_package(ZLIB REQUIRED)
message("ZLIB_INCLUDE_DIRS: ${ZLIB_INCLUDE_DIRS}")
message("ZLIB_LIBRARIES: ${ZLIB_LIBRARIES}")

add_executable(hdfeos5_cmake_experiment hdfeos5_cmake_experiment.c)
target_include_directories(hdfeos5_cmake_experiment PRIVATE ${HDF5_INCLUDE_DIRS} ${HDFEOS5_INCLUDE_DIRS})
target_link_libraries(hdfeos5_cmake_experiment PRIVATE ${HDF5_C_LIBRARIES} ${HDFEOS5_C_LIBRARIES})

add_executable(he5_gd_writedata he5_gd_writedata.c)
target_include_directories(he5_gd_writedata PRIVATE ${HDF5_INCLUDE_DIRS} ${ZLIB_INCLUDE_DIRS} ${HDFEOS5_INCLUDE_DIRS})
target_link_libraries(he5_gd_writedata PRIVATE ${HDF5_C_LIBRARIES} ${HDF5_C_HL_LIBRARIES} ${ZLIB_LIBRARIES} ${HDFEOS5_C_LIBRARIES})

add_executable(TestGrid TestGrid.c)
target_include_directories(TestGrid PRIVATE ${HDF5_INCLUDE_DIRS} ${ZLIB_INCLUDE_DIRS} ${HDFEOS5_INCLUDE_DIRS})
target_link_libraries(TestGrid PRIVATE ${HDF5_C_LIBRARIES} ${HDF5_C_HL_LIBRARIES} ${ZLIB_LIBRARIES} ${HDFEOS5_C_LIBRARIES})

add_executable(grid_1_2d grid_1_2d.c)
target_include_directories(grid_1_2d PRIVATE ${HDF5_INCLUDE_DIRS} ${ZLIB_INCLUDE_DIRS} ${HDFEOS5_INCLUDE_DIRS})
target_link_libraries(grid_1_2d PRIVATE ${HDF5_C_LIBRARIES} ${HDF5_C_HL_LIBRARIES} ${ZLIB_LIBRARIES} ${HDFEOS5_C_LIBRARIES})

