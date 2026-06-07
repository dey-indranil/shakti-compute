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
