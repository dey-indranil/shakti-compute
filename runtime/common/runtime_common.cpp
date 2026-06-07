#include "shakti/runtime.h"

#include "common/backend.h"

#include <cstdlib>
#include <cstring>

namespace {

constexpr size_t kBackendCount = 3;

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

shakti::Backend* backendByIndex(size_t index) {
  switch (index) {
    case 0:
      return &shakti::cpuBackend();
    case 1:
      return &shakti::cudaBackend();
    case 2:
      return &shakti::hipBackend();
    default:
      return nullptr;
  }
}

void fillBackendInfo(const shakti::Backend& backend, ShaktiBackendInfo* info) {
  info->name = backend.name();
  info->available = backend.isAvailable() ? 1 : 0;
  info->supports_memory = backend.supportsMemory() ? 1 : 0;
  info->supports_launch = backend.supportsLaunch() ? 1 : 0;
  info->supports_streams = backend.supportsStreams() ? 1 : 0;
  info->supports_events = backend.supportsEvents() ? 1 : 0;
  info->status_message = backend.statusMessage();
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

ShaktiResult shaktiLaunchKernel(ShaktiKernelFn kernel, ShaktiDim3 grid_dim,
                                ShaktiDim3 block_dim, void* args,
                                size_t shared_memory_bytes) {
  shakti::Backend* backend = selectedBackend();
  ShaktiResult backend_status = ensureUsableBackend(backend);
  if (backend_status != SHAKTI_SUCCESS) {
    return backend_status;
  }

  return backend->launchKernel(kernel, grid_dim, block_dim, args, shared_memory_bytes);
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

size_t shaktiGetBackendCount(void) {
  return kBackendCount;
}

ShaktiResult shaktiGetBackendInfo(size_t index, ShaktiBackendInfo* info) {
  if (info == nullptr) {
    return SHAKTI_ERROR_INVALID_VALUE;
  }

  shakti::Backend* backend = backendByIndex(index);
  if (backend == nullptr) {
    return SHAKTI_ERROR_INVALID_VALUE;
  }

  fillBackendInfo(*backend, info);
  return SHAKTI_SUCCESS;
}

ShaktiResult shaktiGetSelectedBackendInfo(ShaktiBackendInfo* info) {
  if (info == nullptr) {
    return SHAKTI_ERROR_INVALID_VALUE;
  }

  shakti::Backend* backend = selectedBackend();
  if (backend == nullptr) {
    return SHAKTI_ERROR_INVALID_VALUE;
  }

  fillBackendInfo(*backend, info);
  return SHAKTI_SUCCESS;
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
