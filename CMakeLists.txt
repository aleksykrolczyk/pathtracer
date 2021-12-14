cmake_minimum_required (VERSION 3.13)


# Workaround for the lab computers.
if (WIN32)
  set (LUGGCGL_TMP_DIR ${CMAKE_SOURCE_DIR}/../Temp)
  if (NOT EXISTS ${LUGGCGL_TMP_DIR})
    file (MAKE_DIRECTORY ${LUGGCGL_TMP_DIR})
  endif ()
  set (ENV{TMP} ${LUGGCGL_TMP_DIR})
endif ()


# Set project attributes.
project(
  raytracer
  VERSION 2021.0
  DESCRIPTION [[Repository for the first assignment of Computer Graphics course EDAN35 at Lund University, Sweden.]]
  HOMEPAGE_URL [[https://github.com/LUGGPublic/Lab1-RayTracing]]
  LANGUAGES CXX C
)

set(CMAKE_C_COMPILER "/opt/homebrew/opt/llvm/bin/clang")
set(CMAKE_CXX_COMPILER "/opt/homebrew/opt/llvm/bin/clang++")
set(OPENMP_LIBRARIES "/opt/homebrew/opt/llvm/lib")
set(OPENMP_INCLUDES "/opt/homebrew/opt/llvm/include")

if(CMAKE_C_COMPILER_ID MATCHES "Clang")
  set(OpenMP_C "${CMAKE_C_COMPILER}")
  set(OpenMP_C_FLAGS "-fopenmp=libomp -Wno-unused-command-line-argument")
  set(OpenMP_C_LIB_NAMES "libomp" "libgomp" "libiomp5")
  set(OpenMP_libomp_LIBRARY ${OpenMP_C_LIB_NAMES})
  set(OpenMP_libgomp_LIBRARY ${OpenMP_C_LIB_NAMES})
  set(OpenMP_libiomp5_LIBRARY ${OpenMP_C_LIB_NAMES})
endif()
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set(OpenMP_CXX "${CMAKE_CXX_COMPILER}")
  set(OpenMP_CXX_FLAGS "-fopenmp=libomp -Wno-unused-command-line-argument")
  set(OpenMP_CXX_LIB_NAMES "libomp" "libgomp" "libiomp5")
  set(OpenMP_libomp_LIBRARY ${OpenMP_CXX_LIB_NAMES})
  set(OpenMP_libgomp_LIBRARY ${OpenMP_CXX_LIB_NAMES})
  set(OpenMP_libiomp5_LIBRARY ${OpenMP_CXX_LIB_NAMES})
endif()

find_package ( OpenMP REQUIRED )

include_directories("${OPENMP_INCLUDES}")
link_directories("${OPENMP_LIBRARIES}")
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")

# Set build type.
set (CMAKE_CONFIGURATION_TYPES [[Debug;Release;RelWithDebInfo]])
if (NOT CMAKE_BUILD_TYPE)
  set (
    CMAKE_BUILD_TYPE
    [[Release]]
    CACHE STRING
    "Choose the type of build, options are: ${CMAKE_CONFIGURATION_TYPES}."
    FORCE
  )
endif (NOT CMAKE_BUILD_TYPE)
set_property (CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS ${CMAKE_CONFIGURATION_TYPES})

# Set up the executable.
add_executable(raytracer Triangle.cpp Triangle.h Intersectable.h CONSTS.h)
target_sources(
  raytracer
  PRIVATE
    [[main.cpp]]
    [[stb_image.h]]
    [[stb_image_write.h]]
    [[swCamera.cpp]]
    [[swCamera.h]]
    [[swCamera.cpp]]
    [[swCamera.h]]
    [[swIntersection.cpp]]
    [[swIntersection.h]]
    [[swMaterial.h]]
    [[swRay.h]]
    [[swScene.cpp]]
    [[swScene.h]]
    [[swSphere.cpp]]
    [[swSphere.h]]
    [[swVec3.h]]
)
target_compile_definitions(
  raytracer
  PRIVATE
    $<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CXX_COMPILER_ID:MSVC>>:NOMINMAX>
)
target_compile_features(raytracer PRIVATE cxx_std_11)
target_compile_options(
  raytracer
  PRIVATE
    $<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CXX_COMPILER_ID:MSVC>>:/utf-8;/Zc:__cplusplus>
)

