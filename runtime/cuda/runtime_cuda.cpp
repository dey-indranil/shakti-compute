#include "common/backend.h"

#if SHAKTI_HAS_CUDA_RUNTIME
#include <cuda_runtime_api.h>
#endif

namespace {

#if SHAKTI_HAS_CUDA_RUNTIME
const char* cudaStatusMessage() {
  int device_count = 0;
  cudaError_t result = cudaGetDeviceCount(&device_count);
  switch (result) {
    case cudaSuccess:
      if (device_count > 0) {
        return "CUDA backend has runtime memory support";
      }
      return "CUDA runtime found no CUDA devices";
    case cudaErrorNoDevice:
      return "CUDA runtime found no CUDA devices";
    case cudaErrorInsufficientDriver:
      return "CUDA driver is missing or insufficient for the CUDA runtime";
    default:
      return cudaGetErrorString(result);
  }
}

ShaktiResult mapCudaResult(cudaError_t result) {
  switch (result) {
    case cudaSuccess:
      return SHAKTI_SUCCESS;
    case cudaErrorInvalidValue:
      return SHAKTI_ERROR_INVALID_VALUE;
    case cudaErrorMemoryAllocation:
      return SHAKTI_ERROR_OUT_OF_MEMORY;
    case cudaErrorNoDevice:
    case cudaErrorInsufficientDriver:
      return SHAKTI_ERROR_UNAVAILABLE;
    default:
      return SHAKTI_ERROR_UNKNOWN;
  }
}

bool isValidMemcpyKind(ShaktiMemcpyKind kind) {
  switch (kind) {
    case SHAKTI_MEMCPY_HOST_TO_HOST:
    case SHAKTI_MEMCPY_HOST_TO_DEVICE:
    case SHAKTI_MEMCPY_DEVICE_TO_HOST:
    case SHAKTI_MEMCPY_DEVICE_TO_DEVICE:
      return true;
    default:
      return false;
  }
}

cudaMemcpyKind toCudaMemcpyKind(ShaktiMemcpyKind kind) {
  switch (kind) {
    case SHAKTI_MEMCPY_HOST_TO_HOST:
      return cudaMemcpyHostToHost;
    case SHAKTI_MEMCPY_HOST_TO_DEVICE:
      return cudaMemcpyHostToDevice;
    case SHAKTI_MEMCPY_DEVICE_TO_HOST:
      return cudaMemcpyDeviceToHost;
    case SHAKTI_MEMCPY_DEVICE_TO_DEVICE:
      return cudaMemcpyDeviceToDevice;
    default:
      return cudaMemcpyDefault;
  }
}

bool hasCudaDevice() {
  int device_count = 0;
  return cudaGetDeviceCount(&device_count) == cudaSuccess && device_count > 0;
}
#else
const char* cudaStatusMessage() {
  return "CUDA backend was not built; configure with SHAKTI_ENABLE_CUDA=ON";
}
#endif

class CudaBackend final : public shakti::Backend {
 public:
  const char* name() const override {
    return "cuda";
  }

  bool isAvailable() const override {
#if SHAKTI_HAS_CUDA_RUNTIME
    return hasCudaDevice();
#else
    return false;
#endif
  }

  bool supportsMemory() const override {
#if SHAKTI_HAS_CUDA_RUNTIME
    return hasCudaDevice();
#else
    return false;
#endif
  }

  bool supportsLaunch() const override {
    return false;
  }

  bool supportsStreams() const override {
    return false;
  }

  bool supportsEvents() const override {
    return false;
  }

  const char* statusMessage() const override {
    return cudaStatusMessage();
  }

  ShaktiResult malloc(void** ptr, size_t bytes) override {
    if (ptr != nullptr) {
      *ptr = nullptr;
    }
#if SHAKTI_HAS_CUDA_RUNTIME
    if (ptr == nullptr) {
      return SHAKTI_ERROR_INVALID_VALUE;
    }

    if (bytes == 0) {
      return SHAKTI_SUCCESS;
    }

    return mapCudaResult(cudaMalloc(ptr, bytes));
#else
    return SHAKTI_ERROR_UNAVAILABLE;
#endif
  }

  ShaktiResult free(void* ptr) override {
#if SHAKTI_HAS_CUDA_RUNTIME
    return mapCudaResult(cudaFree(ptr));
#else
    (void)ptr;
    return SHAKTI_ERROR_UNAVAILABLE;
#endif
  }

  ShaktiResult memcpy(void* dst, const void* src, size_t bytes,
                      ShaktiMemcpyKind kind) override {
#if SHAKTI_HAS_CUDA_RUNTIME
    if (!isValidMemcpyKind(kind)) {
      return SHAKTI_ERROR_INVALID_VALUE;
    }

    if (bytes == 0) {
      return SHAKTI_SUCCESS;
    }

    if (dst == nullptr || src == nullptr) {
      return SHAKTI_ERROR_INVALID_VALUE;
    }

    return mapCudaResult(cudaMemcpy(dst, src, bytes, toCudaMemcpyKind(kind)));
#else
    (void)dst;
    (void)src;
    (void)bytes;
    (void)kind;
    return SHAKTI_ERROR_UNAVAILABLE;
#endif
  }

  ShaktiResult launchKernel(ShaktiKernelFn /*kernel*/, ShaktiDim3 /*grid_dim*/,
                            ShaktiDim3 /*block_dim*/, void* /*args*/,
                            size_t /*shared_memory_bytes*/) override {
    return SHAKTI_ERROR_UNAVAILABLE;
  }

  ShaktiResult deviceSynchronize() override {
#if SHAKTI_HAS_CUDA_RUNTIME
    return mapCudaResult(cudaDeviceSynchronize());
#else
    return SHAKTI_ERROR_UNAVAILABLE;
#endif
  }
};

}  // namespace

namespace shakti {

Backend& cudaBackend() {
  static CudaBackend backend;
  return backend;
}

}  // namespace shakti
