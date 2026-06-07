#include "common/host_memory_backend.h"

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

}  // namespace

namespace shakti {

bool HostMemoryBackend::supportsMemory() const {
  return true;
}

bool HostMemoryBackend::supportsStreams() const {
  return false;
}

bool HostMemoryBackend::supportsEvents() const {
  return false;
}

ShaktiResult HostMemoryBackend::malloc(void** ptr, size_t bytes) {
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

ShaktiResult HostMemoryBackend::free(void* ptr) {
  std::free(ptr);
  return SHAKTI_SUCCESS;
}

ShaktiResult HostMemoryBackend::memcpy(void* dst, const void* src, size_t bytes,
                                       ShaktiMemcpyKind kind) {
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

ShaktiResult HostMemoryBackend::deviceSynchronize() {
  return SHAKTI_SUCCESS;
}

}  // namespace shakti
