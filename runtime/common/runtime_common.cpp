#include "shakti/runtime.h"

#include "common/backend.h"

#include <cstdlib>
#include <cstring>

namespace {

bool isEmpty(const char* value) {
  return value == nullptr || std::strcmp(value, "") == 0;
}

shakti::Backend* selectedBackend() {
  const char* requested_backend = std::getenv("SHAKTI_BACKEND");
  if (isEmpty(requested_backend) || std::strcmp(requested_backend, "cpu") == 0) {
    return &shakti::cpuBackend();
  }

  if (std::strcmp(requested_backend, "cuda") == 0) {
    return &shakti::cudaBackend();
  }

  if (std::strcmp(requested_backend, "hip") == 0) {
    return &shakti::hipBackend();
  }

  return nullptr;
}

ShaktiResult ensureUsableBackend(shakti::Backend* backend) {
  if (backend == nullptr) {
    return SHAKTI_ERROR_INVALID_VALUE;
  }

  if (!backend->isAvailable()) {
    return SHAKTI_ERROR_UNAVAILABLE;
  }

  return SHAKTI_SUCCESS;
}

}  // namespace

extern "C" {

ShaktiResult shaktiMalloc(void** ptr, size_t bytes) {
  shakti::Backend* backend = selectedBackend();
  ShaktiResult backend_status = ensureUsableBackend(backend);
  if (backend_status != SHAKTI_SUCCESS) {
    if (ptr != nullptr) {
      *ptr = nullptr;
    }
    return backend_status;
  }

  return backend->malloc(ptr, bytes);
}

ShaktiResult shaktiFree(void* ptr) {
  shakti::Backend* backend = selectedBackend();
  ShaktiResult backend_status = ensureUsableBackend(backend);
  if (backend_status != SHAKTI_SUCCESS) {
    return backend_status;
  }

  return backend->free(ptr);
}

ShaktiResult shaktiMemcpy(void* dst, const void* src, size_t bytes, ShaktiMemcpyKind kind) {
  shakti::Backend* backend = selectedBackend();
  ShaktiResult backend_status = ensureUsableBackend(backend);
  if (backend_status != SHAKTI_SUCCESS) {
    return backend_status;
  }

  return backend->memcpy(dst, src, bytes, kind);
}

ShaktiResult shaktiDeviceSynchronize(void) {
  shakti::Backend* backend = selectedBackend();
  ShaktiResult backend_status = ensureUsableBackend(backend);
  if (backend_status != SHAKTI_SUCCESS) {
    return backend_status;
  }

  return backend->deviceSynchronize();
}

const char* shaktiGetBackendName(void) {
  shakti::Backend* backend = selectedBackend();
  if (backend == nullptr) {
    return "unknown";
  }

  return backend->name();
}

int shaktiIsBackendAvailable(const char* name) {
  if (isEmpty(name)) {
    return 0;
  }

  if (std::strcmp(name, "cpu") == 0) {
    return shakti::cpuBackend().isAvailable() ? 1 : 0;
  }

  if (std::strcmp(name, "cuda") == 0) {
    return shakti::cudaBackend().isAvailable() ? 1 : 0;
  }

  if (std::strcmp(name, "hip") == 0) {
    return shakti::hipBackend().isAvailable() ? 1 : 0;
  }

  return 0;
}

const char* shaktiGetErrorString(ShaktiResult result) {
  switch (result) {
    case SHAKTI_SUCCESS:
      return "success";
    case SHAKTI_ERROR_INVALID_VALUE:
      return "invalid value";
    case SHAKTI_ERROR_OUT_OF_MEMORY:
      return "out of memory";
    case SHAKTI_ERROR_UNAVAILABLE:
      return "backend unavailable";
    case SHAKTI_ERROR_UNKNOWN:
      return "unknown error";
    default:
      return "unrecognized ShaktiResult";
  }
}

}  // extern "C"
