#ifndef SHAKTI_RUNTIME_COMMON_HOST_MEMORY_BACKEND_H_
#define SHAKTI_RUNTIME_COMMON_HOST_MEMORY_BACKEND_H_

#include "common/backend.h"

namespace shakti {

class HostMemoryBackend : public Backend {
 public:
  bool supportsMemory() const override;
  bool supportsStreams() const override;
  bool supportsEvents() const override;

  ShaktiResult malloc(void** ptr, size_t bytes) override;
  ShaktiResult free(void* ptr) override;
  ShaktiResult memcpy(void* dst, const void* src, size_t bytes,
                      ShaktiMemcpyKind kind) override;
  ShaktiResult deviceSynchronize() override;
};

}  // namespace shakti

#endif
