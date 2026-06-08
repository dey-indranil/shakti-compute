#include "shakti/runtime.h"

#include "common/allocation_registry.h"
#include "common/backend.h"
#include "common/backend_registry.h"

#include <cstring>

namespace {

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

ShaktiResult ensureAllocationOwnedBy(const void* ptr,
                                     const shakti::Backend& backend) {
  const char* owner = shakti::allocationOwner(const_cast<void*>(ptr));
  if (owner == nullptr) {
    return SHAKTI_SUCCESS;
  }

  if (std::strcmp(owner, backend.name()) != 0) {
    return SHAKTI_ERROR_INVALID_VALUE;
  }

  return SHAKTI_SUCCESS;
}

ShaktiResult ensureMemcpyOwnership(void* dst, const void* src, size_t bytes,
                                   const shakti::Backend& backend) {
  if (bytes == 0) {
    return SHAKTI_SUCCESS;
  }

  ShaktiResult dst_status = ensureAllocationOwnedBy(dst, backend);
  if (dst_status != SHAKTI_SUCCESS) {
    return dst_status;
  }

  return ensureAllocationOwnedBy(src, backend);
}

}  // namespace

extern "C" {

ShaktiResult shaktiMalloc(void** ptr, size_t bytes) {
  shakti::Backend* backend = shakti::selectedBackend();
  ShaktiResult backend_status = ensureUsableBackend(backend);
  if (backend_status != SHAKTI_SUCCESS) {
    if (ptr != nullptr) {
      *ptr = nullptr;
    }
    return backend_status;
  }

  ShaktiResult result = backend->malloc(ptr, bytes);
  if (result != SHAKTI_SUCCESS || ptr == nullptr || *ptr == nullptr) {
    return result;
  }

  if (!shakti::registerAllocation(*ptr, backend->name())) {
    backend->free(*ptr);
    *ptr = nullptr;
    return SHAKTI_ERROR_OUT_OF_MEMORY;
  }

  return SHAKTI_SUCCESS;
}

ShaktiResult shaktiFree(void* ptr) {
  shakti::Backend* backend = shakti::selectedBackend();
  ShaktiResult backend_status = ensureUsableBackend(backend);
  if (backend_status != SHAKTI_SUCCESS) {
    return backend_status;
  }

  ShaktiResult owner_status = ensureAllocationOwnedBy(ptr, *backend);
  if (owner_status != SHAKTI_SUCCESS) {
    return owner_status;
  }

  ShaktiResult result = backend->free(ptr);
  if (result == SHAKTI_SUCCESS) {
    shakti::unregisterAllocation(ptr);
  }

  return result;
}

ShaktiResult shaktiMemcpy(void* dst, const void* src, size_t bytes, ShaktiMemcpyKind kind) {
  shakti::Backend* backend = shakti::selectedBackend();
  ShaktiResult backend_status = ensureUsableBackend(backend);
  if (backend_status != SHAKTI_SUCCESS) {
    return backend_status;
  }

  ShaktiResult owner_status = ensureMemcpyOwnership(dst, src, bytes, *backend);
  if (owner_status != SHAKTI_SUCCESS) {
    return owner_status;
  }

  return backend->memcpy(dst, src, bytes, kind);
}

ShaktiResult shaktiLaunchKernel(ShaktiKernelFn kernel, ShaktiDim3 grid_dim,
                                ShaktiDim3 block_dim, void* args,
                                size_t shared_memory_bytes) {
  shakti::Backend* backend = shakti::selectedBackend();
  ShaktiResult backend_status = ensureUsableBackend(backend);
  if (backend_status != SHAKTI_SUCCESS) {
    return backend_status;
  }

  return backend->launchKernel(kernel, grid_dim, block_dim, args, shared_memory_bytes);
}

ShaktiResult shaktiDeviceSynchronize(void) {
  shakti::Backend* backend = shakti::selectedBackend();
  ShaktiResult backend_status = ensureUsableBackend(backend);
  if (backend_status != SHAKTI_SUCCESS) {
    return backend_status;
  }

  return backend->deviceSynchronize();
}

const char* shaktiGetBackendName(void) {
  shakti::Backend* backend = shakti::selectedBackend();
  if (backend == nullptr) {
    return "unknown";
  }

  return backend->name();
}

int shaktiIsBackendAvailable(const char* name) {
  shakti::Backend* backend = shakti::backendByName(name);
  return backend != nullptr && backend->isAvailable() ? 1 : 0;
}

size_t shaktiGetBackendCount(void) {
  return shakti::backendCount();
}

ShaktiResult shaktiGetBackendInfo(size_t index, ShaktiBackendInfo* info) {
  if (info == nullptr) {
    return SHAKTI_ERROR_INVALID_VALUE;
  }

  shakti::Backend* backend = shakti::backendAt(index);
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

  shakti::Backend* backend = shakti::selectedBackend();
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
