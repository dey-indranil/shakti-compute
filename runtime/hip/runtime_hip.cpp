#include "common/backend.h"

#if SHAKTI_HAS_HIP_RUNTIME
#include <hip/hip_runtime_api.h>
#endif

namespace {

#if SHAKTI_HAS_HIP_RUNTIME
const char* hipStatusMessage() {
  int device_count = 0;
  hipError_t result = hipGetDeviceCount(&device_count);
  switch (result) {
    case hipSuccess:
      if (device_count > 0) {
        return "HIP backend has runtime memory support";
      }
      return "HIP runtime found no HIP devices";
    case hipErrorNoDevice:
      return "HIP runtime found no HIP devices";
    default:
      return hipGetErrorString(result);
  }
}

ShaktiResult mapHipResult(hipError_t result) {
  switch (result) {
    case hipSuccess:
      return SHAKTI_SUCCESS;
    case hipErrorInvalidValue:
      return SHAKTI_ERROR_INVALID_VALUE;
    case hipErrorOutOfMemory:
      return SHAKTI_ERROR_OUT_OF_MEMORY;
    case hipErrorNoDevice:
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

hipMemcpyKind toHipMemcpyKind(ShaktiMemcpyKind kind) {
  switch (kind) {
    case SHAKTI_MEMCPY_HOST_TO_HOST:
      return hipMemcpyHostToHost;
    case SHAKTI_MEMCPY_HOST_TO_DEVICE:
      return hipMemcpyHostToDevice;
    case SHAKTI_MEMCPY_DEVICE_TO_HOST:
      return hipMemcpyDeviceToHost;
    case SHAKTI_MEMCPY_DEVICE_TO_DEVICE:
      return hipMemcpyDeviceToDevice;
    default:
      return hipMemcpyDefault;
  }
}

bool hasHipDevice() {
  int device_count = 0;
  return hipGetDeviceCount(&device_count) == hipSuccess && device_count > 0;
}
#else
const char* hipStatusMessage() {
  return "HIP backend was not built; configure with SHAKTI_ENABLE_HIP=ON";
}
#endif

class HipBackend final : public shakti::Backend {
 public:
  const char* name() const override {
    return "hip";
  }

  bool isAvailable() const override {
#if SHAKTI_HAS_HIP_RUNTIME
    return hasHipDevice();
#else
    return false;
#endif
  }

  bool supportsMemory() const override {
#if SHAKTI_HAS_HIP_RUNTIME
    return hasHipDevice();
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
    return hipStatusMessage();
  }

  ShaktiResult malloc(void** ptr, size_t bytes) override {
    if (ptr != nullptr) {
      *ptr = nullptr;
    }
#if SHAKTI_HAS_HIP_RUNTIME
    if (ptr == nullptr) {
      return SHAKTI_ERROR_INVALID_VALUE;
    }

    if (bytes == 0) {
      return SHAKTI_SUCCESS;
    }

    return mapHipResult(hipMalloc(ptr, bytes));
#else
    (void)bytes;
    return SHAKTI_ERROR_UNAVAILABLE;
#endif
  }

  ShaktiResult free(void* ptr) override {
#if SHAKTI_HAS_HIP_RUNTIME
    return mapHipResult(hipFree(ptr));
#else
    (void)ptr;
    return SHAKTI_ERROR_UNAVAILABLE;
#endif
  }

  ShaktiResult memcpy(void* dst, const void* src, size_t bytes,
                      ShaktiMemcpyKind kind) override {
#if SHAKTI_HAS_HIP_RUNTIME
    if (!isValidMemcpyKind(kind)) {
      return SHAKTI_ERROR_INVALID_VALUE;
    }

    if (bytes == 0) {
      return SHAKTI_SUCCESS;
    }

    if (dst == nullptr || src == nullptr) {
      return SHAKTI_ERROR_INVALID_VALUE;
    }

    return mapHipResult(hipMemcpy(dst, src, bytes, toHipMemcpyKind(kind)));
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
#if SHAKTI_HAS_HIP_RUNTIME
    return mapHipResult(hipDeviceSynchronize());
#else
    return SHAKTI_ERROR_UNAVAILABLE;
#endif
  }
};

}  // namespace

namespace shakti {

Backend& hipBackend() {
  static HipBackend backend;
  return backend;
}

}  // namespace shakti
