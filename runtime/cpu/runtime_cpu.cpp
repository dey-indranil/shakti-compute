#include "common/backend.h"

#include <cstdlib>
#include <cstring>

namespace {

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

class CpuBackend final : public shakti::Backend {
 public:
  const char* name() const override {
    return "cpu";
  }

  bool isAvailable() const override {
    return true;
  }

  ShaktiResult malloc(void** ptr, size_t bytes) override {
    if (ptr == nullptr) {
      return SHAKTI_ERROR_INVALID_VALUE;
    }

    *ptr = nullptr;
    if (bytes == 0) {
      return SHAKTI_SUCCESS;
    }

    void* allocation = std::malloc(bytes);
    if (allocation == nullptr) {
      return SHAKTI_ERROR_OUT_OF_MEMORY;
    }

    *ptr = allocation;
    return SHAKTI_SUCCESS;
  }

  ShaktiResult free(void* ptr) override {
    std::free(ptr);
    return SHAKTI_SUCCESS;
  }

  ShaktiResult memcpy(void* dst, const void* src, size_t bytes,
                      ShaktiMemcpyKind kind) override {
    if (!isValidMemcpyKind(kind)) {
      return SHAKTI_ERROR_INVALID_VALUE;
    }

    if (bytes == 0) {
      return SHAKTI_SUCCESS;
    }

    if (dst == nullptr || src == nullptr) {
      return SHAKTI_ERROR_INVALID_VALUE;
    }

    std::memcpy(dst, src, bytes);
    return SHAKTI_SUCCESS;
  }

  ShaktiResult launchKernel(ShaktiKernelFn kernel, ShaktiDim3 grid_dim,
                            ShaktiDim3 block_dim, void* args,
                            size_t shared_memory_bytes) override {
    if (kernel == nullptr || grid_dim.x == 0 || grid_dim.y == 0 || grid_dim.z == 0 ||
        block_dim.x == 0 || block_dim.y == 0 || block_dim.z == 0) {
      return SHAKTI_ERROR_INVALID_VALUE;
    }

    ShaktiLaunchContext context = {grid_dim, block_dim, shared_memory_bytes};
    kernel(args, &context);
    return SHAKTI_SUCCESS;
  }

  ShaktiResult deviceSynchronize() override {
    return SHAKTI_SUCCESS;
  }
};

}  // namespace

namespace shakti {

Backend& cpuBackend() {
  static CpuBackend backend;
  return backend;
}

}  // namespace shakti
