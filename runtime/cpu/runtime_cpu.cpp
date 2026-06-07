#include "common/host_memory_backend.h"

namespace {

class CpuBackend final : public shakti::HostMemoryBackend {
 public:
  const char* name() const override {
    return "cpu";
  }

  bool isAvailable() const override {
    return true;
  }

  bool supportsMemory() const override {
    return true;
  }

  bool supportsLaunch() const override {
    return true;
  }

  bool supportsStreams() const override {
    return false;
  }

  bool supportsEvents() const override {
    return false;
  }

  const char* statusMessage() const override {
    return "CPU backend is available";
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
