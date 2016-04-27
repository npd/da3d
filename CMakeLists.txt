cmake_minimum_required (VERSION 2.8)
project (da3d)

# The build type "Release" adds some optimizations
if (NOT CMAKE_BUILD_TYPE)
  set (CMAKE_BUILD_TYPE "Release")
endif ()

# GCC on MacOs needs this option to use the clang assembler
if ((CMAKE_CXX_COMPILER_ID STREQUAL "GNU") AND (APPLE))
  set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wa,-q")
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wa,-q")
endif ()

# Optimize to the current CPU and enable warnings
if ((CMAKE_CXX_COMPILER_ID STREQUAL "GNU") OR
    (CMAKE_CXX_COMPILER_ID STREQUAL "Clang") OR
    (CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang"))
  set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=native -Wall -Wextra")
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native -Wall -Wextra")
endif ()

# Enable C99
if (CMAKE_VERSION VERSION_LESS "3.1")
  set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99")
else ()
  set (CMAKE_C_STANDARD 99)
endif ()

# Enable C++11
if (CMAKE_VERSION VERSION_LESS "3.1")
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")
else ()
  set (CMAKE_CXX_STANDARD 11)
endif ()

# Enable OpenMP
find_package (OpenMP)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")

# Link LibFFTW
find_path (FFTW_INCLUDE_DIR fftw3.h)
find_library (FFTWF_LIBRARIES NAMES fftw3f)
include_directories (PUBLIC ${FFTW_INCLUDE_DIR})
link_libraries (${FFTWF_LIBRARIES})
if (NOT FFTW_INCLUDE_DIR)
  message (FATAL_ERROR "FFTW3 not found.")
endif ()

# Enable Eigen
find_path (EIGEN3_INCLUDE_DIR NAMES signature_of_eigen3_matrix_library PATH_SUFFIXES eigen3 eigen)
include_directories (PUBLIC ${EIGEN3_INCLUDE_DIR})
if (NOT EIGEN3_INCLUDE_DIR)
  message (FATAL_ERROR "Eigen3 not found.")
endif ()

# Link LibTIFF, LibJPEG, LibPNG
find_package (TIFF REQUIRED)
find_package (JPEG REQUIRED)
find_package (PNG REQUIRED)
include_directories (PUBLIC ${TIFF_INCLUDE_DIR} PUBLIC ${JPEG_INCLUDE_DIR} PUBLIC ${PNG_INCLUDE_DIRS})
link_libraries (${TIFF_LIBRARIES} ${JPEG_LIBRARIES} ${PNG_LIBRARIES})

set_property (SOURCE iio.c PROPERTY COMPILE_FLAGS "-Wno-unused-variable -Wno-unused-parameter -Wno-pointer-sign -Wno-parentheses -Wno-deprecated-declarations -Wno-unused-function")

add_executable(da3d DA3D.cpp DA3D.hpp DftPatch.hpp iio.c iio.h WeightMap.cpp WeightMap.hpp Utils.cpp Utils.hpp main.cpp)
