#include "shakti/runtime.h"

#include "common/backend.h"

#include <cstdlib>
#include <cstring>

namespace {

shakti::Backend* selectedBackend() {
  const char* requested_backend = std::getenv("SHAKTI_BACKEND");
  if (requested_backend == nullptr || std::strcmp(requested_backend, "") == 0 ||
      std::strcmp(requested_backend, "cpu") == 0) {
    return &shakti::cpuBackend();
  }

  return nullptr;
}

}  // namespace

extern "C" {

ShaktiResult shaktiMalloc(void** ptr, size_t bytes) {
  shakti::Backend* backend = selectedBackend();
  if (backend == nullptr) {
    if (ptr != nullptr) {
      *ptr = nullptr;
    }
    return SHAKTI_ERROR_INVALID_VALUE;
  }

  return backend->malloc(ptr, bytes);
}

ShaktiResult shaktiFree(void* ptr) {
  shakti::Backend* backend = selectedBackend();
  if (backend == nullptr) {
    return SHAKTI_ERROR_INVALID_VALUE;
  }

  return backend->free(ptr);
}

ShaktiResult shaktiMemcpy(void* dst, const void* src, size_t bytes, ShaktiMemcpyKind kind) {
  shakti::Backend* backend = selectedBackend();
  if (backend == nullptr) {
    return SHAKTI_ERROR_INVALID_VALUE;
  }

  return backend->memcpy(dst, src, bytes, kind);
}

ShaktiResult shaktiDeviceSynchronize(void) {
  shakti::Backend* backend = selectedBackend();
  if (backend == nullptr) {
    return SHAKTI_ERROR_INVALID_VALUE;
  }

  return backend->deviceSynchronize();
}

const char* shaktiGetErrorString(ShaktiResult result) {
  switch (result) {
    case SHAKTI_SUCCESS:
      return "success";
    case SHAKTI_ERROR_INVALID_VALUE:
      return "invalid value";
    case SHAKTI_ERROR_OUT_OF_MEMORY:
      return "out of memory";
    case SHAKTI_ERROR_UNKNOWN:
      return "unknown error";
    default:
      return "unrecognized ShaktiResult";
  }
}

}  // extern "C"
