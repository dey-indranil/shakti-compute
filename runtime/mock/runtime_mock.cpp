#include "common/host_memory_backend.h"

namespace {

class MockBackend final : public shakti::HostMemoryBackend {
 public:
  const char* name() const override {
    return "mock_gpu";
  }

  bool isAvailable() const override {
    return true;
  }

  bool supportsLaunch() const override {
    return false;
  }

  const char* statusMessage() const override {
    return "Mock GPU backend uses host memory for hardware-free backend tests";
  }

  ShaktiResult launchKernel(ShaktiKernelFn /*kernel*/, ShaktiDim3 /*grid_dim*/,
                            ShaktiDim3 /*block_dim*/, void* /*args*/,
                            size_t /*shared_memory_bytes*/) override {
    return SHAKTI_ERROR_UNAVAILABLE;
  }
};

}  // namespace

namespace shakti {

Backend& mockBackend() {
  static MockBackend backend;
  return backend;
}

}  // namespace shakti
