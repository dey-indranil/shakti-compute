#ifndef SHAKTI_RUNTIME_COMMON_BACKEND_H_
#define SHAKTI_RUNTIME_COMMON_BACKEND_H_

#include <stddef.h>

#include "shakti/types.h"

namespace shakti {

class Backend {
 public:
  virtual ~Backend() = default;

  virtual const char* name() const = 0;
  virtual bool isAvailable() const = 0;
  virtual bool supportsMemory() const = 0;
  virtual bool supportsLaunch() const = 0;
  virtual bool supportsStreams() const = 0;
  virtual bool supportsEvents() const = 0;
  virtual const char* statusMessage() const = 0;
  virtual ShaktiResult malloc(void** ptr, size_t bytes) = 0;
  virtual ShaktiResult free(void* ptr) = 0;
  virtual ShaktiResult memcpy(void* dst, const void* src, size_t bytes,
                              ShaktiMemcpyKind kind) = 0;
  virtual ShaktiResult launchKernel(ShaktiKernelFn kernel, ShaktiDim3 grid_dim,
                                    ShaktiDim3 block_dim, void* args,
                                    size_t shared_memory_bytes) = 0;
  virtual ShaktiResult deviceSynchronize() = 0;
};

Backend& cpuBackend();
Backend& mockBackend();
Backend& cudaBackend();
Backend& hipBackend();

}  // namespace shakti

#endif
