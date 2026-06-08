cmake_minimum_required(VERSION 3.16)

project(
  ShaktiCompute
  VERSION 1.4.0
  DESCRIPTION "Experimental vendor-neutral accelerated computing runtime"
  LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(SHAKTI_ENABLE_CUDA "Enable CUDA runtime memory support" OFF)
option(SHAKTI_ENABLE_HIP "Enable HIP runtime memory support" OFF)

if(SHAKTI_ENABLE_CUDA)
  find_package(CUDAToolkit REQUIRED)
endif()

if(SHAKTI_ENABLE_HIP)
  find_package(hip REQUIRED CONFIG)
endif()

enable_testing()

add_library(shakti_runtime
  runtime/common/allocation_registry.cpp
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
    SHAKTI_HAS_HIP_RUNTIME=$<BOOL:${SHAKTI_ENABLE_HIP}>
)

if(SHAKTI_ENABLE_CUDA)
  target_link_libraries(shakti_runtime
    PRIVATE
      CUDA::cudart
  )
endif()

if(SHAKTI_ENABLE_HIP)
  if(TARGET hip::host)
    target_link_libraries(shakti_runtime
      PRIVATE
        hip::host
    )
  elseif(TARGET hip::amdhip64)
    target_link_libraries(shakti_runtime
      PRIVATE
        hip::amdhip64
    )
  else()
    message(FATAL_ERROR "HIP package found, but no known HIP host runtime target was exported")
  endif()
endif()

add_subdirectory(examples/saxpy)
add_subdirectory(tests)
