cmake_minimum_required(VERSION 3.16)

project(
  ShaktiCompute
  VERSION 1.0.0
  DESCRIPTION "Experimental vendor-neutral accelerated computing runtime"
  LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(SHAKTI_ENABLE_CUDA "Enable the CUDA backend skeleton" OFF)
option(SHAKTI_ENABLE_HIP "Enable the HIP backend skeleton" OFF)

if(SHAKTI_ENABLE_CUDA)
  find_package(CUDAToolkit REQUIRED)
endif()

enable_testing()

add_library(shakti_runtime
  runtime/common/backend_registry.cpp
  runtime/common/host_memory_backend.cpp
  runtime/common/runtime_common.cpp
  runtime/cpu/runtime_cpu.cpp
  runtime/cuda/runtime_cuda.cpp
  runtime/hip/runtime_hip.cpp
  runtime/mock/runtime_mock.cpp
)

target_include_directories(shakti_runtime
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/runtime
)

target_compile_definitions(shakti_runtime
  PRIVATE
    SHAKTI_HAS_CUDA_RUNTIME=$<BOOL:${SHAKTI_ENABLE_CUDA}>
    SHAKTI_ENABLE_HIP=$<BOOL:${SHAKTI_ENABLE_HIP}>
)

if(SHAKTI_ENABLE_CUDA)
  target_link_libraries(shakti_runtime
    PRIVATE
      CUDA::cudart
  )
endif()

add_subdirectory(examples/saxpy)
add_subdirectory(tests)
