#ifndef SHAKTI_RUNTIME_COMMON_BACKEND_H_
#define SHAKTI_RUNTIME_COMMON_BACKEND_H_

#include <stddef.h>

#include "shakti/types.h"

namespace shakti {

class Backend {
 public:
  virtual ~Backend() = default;

  virtual const char* name() const = 0;
  virtual ShaktiResult malloc(void** ptr, size_t bytes) = 0;
  virtual ShaktiResult free(void* ptr) = 0;
  virtual ShaktiResult memcpy(void* dst, const void* src, size_t bytes,
                              ShaktiMemcpyKind kind) = 0;
  virtual ShaktiResult deviceSynchronize() = 0;
};

Backend& cpuBackend();

}  // namespace shakti

#endif
