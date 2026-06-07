#include "common/backend.h"

namespace {

class HipBackend final : public shakti::Backend {
 public:
  const char* name() const override {
    return "hip";
  }

  bool isAvailable() const override {
    return false;
  }

  bool supportsMemory() const override {
    return false;
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
    return "HIP backend is a skeleton";
  }

  ShaktiResult malloc(void** ptr, size_t /*bytes*/) override {
    if (ptr != nullptr) {
      *ptr = nullptr;
    }
    return SHAKTI_ERROR_UNAVAILABLE;
  }

  ShaktiResult free(void* /*ptr*/) override {
    return SHAKTI_ERROR_UNAVAILABLE;
  }

  ShaktiResult memcpy(void* /*dst*/, const void* /*src*/, size_t /*bytes*/,
                      ShaktiMemcpyKind /*kind*/) override {
    return SHAKTI_ERROR_UNAVAILABLE;
  }

  ShaktiResult launchKernel(ShaktiKernelFn /*kernel*/, ShaktiDim3 /*grid_dim*/,
                            ShaktiDim3 /*block_dim*/, void* /*args*/,
                            size_t /*shared_memory_bytes*/) override {
    return SHAKTI_ERROR_UNAVAILABLE;
  }

  ShaktiResult deviceSynchronize() override {
    return SHAKTI_ERROR_UNAVAILABLE;
  }
};

}  // namespace

namespace shakti {

Backend& hipBackend() {
  static HipBackend backend;
  return backend;
}

}  // namespace shakti
