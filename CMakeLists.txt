CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(da3d)

# The build type "Release" adds some optimizations
if (NOT CMAKE_BUILD_TYPE)
  set (CMAKE_BUILD_TYPE "Release")
endif ()

# Are we using gcc?
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  # GCC on MacOs needs this option to use the clang assembler
  if (APPLE)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wa,-q")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wa,-q")
  endif ()
  # Optimize to the current CPU and enable warnings
  set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=native -Wall -Wextra")
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native -Wall -Wextra")
endif ()

# Enable C99
if (CMAKE_VERSION VERSION_LESS "3.1")
  set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")
else ()
  set (CMAKE_C_STANDARD 99)
endif ()

# Enable C++11
# if (CMAKE_VERSION VERSION_LESS "3.1")
#   set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
# else ()
#   set (CMAKE_CXX_STANDARD 11)
# endif ()

find_package (TIFF REQUIRED)
find_package (JPEG REQUIRED)
find_package (PNG REQUIRED)
find_package (OpenMP REQUIRED)
find_path (FFTW_INCLUDES fftw3.h)
find_library (FFTWF_LIBRARIES NAMES fftw3f)
include_directories (PUBLIC ${TIFF_INCLUDE_DIR} PUBLIC ${JPEG_INCLUDE_DIR} PUBLIC ${PNG_INCLUDE_DIRS} PUBLIC ${FFTW_INCLUDES})
link_libraries (${TIFF_LIBRARIES} ${JPEG_LIBRARIES} ${PNG_LIBRARIES} ${FFTWF_LIBRARIES})
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")

set_property(SOURCE iio.c PROPERTY COMPILE_FLAGS "-Wno-unused-variable -Wno-unused-parameter -Wno-pointer-sign -Wno-parentheses -Wno-deprecated-declarations -Wno-unused-function")

add_executable(da3d DA3D.cc DA3D.h DftPatch.cc DftPatch.h iio.c iio.h IioImage.cc IioImage.h IioWeightMap.cc IioWeightMap.h Image.cc Image.h WeightMap.cc WeightMap.h Utils.h main.cc)
