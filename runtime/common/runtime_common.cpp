#include "shakti/runtime.h"

#include "common/allocation_registry.h"
#include "common/backend.h"
#include "common/backend_registry.h"

#include <cstring>
#include <string>

namespace {

thread_local std::string last_error_message = "success";

void fillBackendInfo(const shakti::Backend& backend, ShaktiBackendInfo* info) {
  info->name = backend.name();
  info->available = backend.isAvailable() ? 1 : 0;
  info->supports_memory = backend.supportsMemory() ? 1 : 0;
  info->supports_launch = backend.supportsLaunch() ? 1 : 0;
  info->supports_streams = backend.supportsStreams() ? 1 : 0;
  info->supports_events = backend.supportsEvents() ? 1 : 0;
  info->status_message = backend.statusMessage();
}

ShaktiResult finish(ShaktiResult result, const std::string& message) {
  last_error_message = message;
  return result;
}

std::string resultMessage(ShaktiResult result) {
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

std::string backendOperationMessage(const char* operation,
                                    const shakti::Backend& backend,
                                    ShaktiResult result) {
  return std::string(operation) + " failed on backend '" + backend.name() +
         "': " + resultMessage(result) + " (" + backend.statusMessage() + ")";
}

ShaktiResult ensureUsableBackend(shakti::Backend* backend, std::string* detail) {
  if (backend == nullptr) {
    if (detail != nullptr) {
      *detail = "selected backend is unknown; check SHAKTI_BACKEND";
    }
    return SHAKTI_ERROR_INVALID_VALUE;
  }

  if (!backend->isAvailable()) {
    if (detail != nullptr) {
      *detail = std::string("backend '") + backend->name() +
                "' is unavailable: " + backend->statusMessage();
    }
    return SHAKTI_ERROR_UNAVAILABLE;
  }

  if (detail != nullptr) {
    *detail = "success";
  }
  return SHAKTI_SUCCESS;
}

ShaktiResult ensureAllocationOwnedBy(const void* ptr,
                                     const shakti::Backend& backend,
                                     std::string* detail) {
  const char* owner = shakti::allocationOwner(const_cast<void*>(ptr));
  if (owner == nullptr) {
    if (detail != nullptr) {
      *detail = "success";
    }
    return SHAKTI_SUCCESS;
  }

  if (std::strcmp(owner, backend.name()) != 0) {
    if (detail != nullptr) {
      *detail = std::string("allocation belongs to backend '") + owner +
                "' but selected backend is '" + backend.name() + "'";
    }
    return SHAKTI_ERROR_INVALID_VALUE;
  }

  if (detail != nullptr) {
    *detail = "success";
  }
  return SHAKTI_SUCCESS;
}

ShaktiResult ensureMemcpyOwnership(void* dst, const void* src, size_t bytes,
                                   const shakti::Backend& backend,
                                   std::string* detail) {
  if (bytes == 0) {
    if (detail != nullptr) {
      *detail = "success";
    }
    return SHAKTI_SUCCESS;
  }

  ShaktiResult dst_status = ensureAllocationOwnedBy(dst, backend, detail);
  if (dst_status != SHAKTI_SUCCESS) {
    return dst_status;
  }

  return ensureAllocationOwnedBy(src, backend, detail);
}

}  // namespace

extern "C" {

ShaktiResult shaktiMalloc(void** ptr, size_t bytes) {
  shakti::Backend* backend = shakti::selectedBackend();
  std::string detail;
  ShaktiResult backend_status = ensureUsableBackend(backend, &detail);
  if (backend_status != SHAKTI_SUCCESS) {
    if (ptr != nullptr) {
      *ptr = nullptr;
    }
    return finish(backend_status, detail);
  }

  ShaktiResult result = backend->malloc(ptr, bytes);
  if (result != SHAKTI_SUCCESS || ptr == nullptr || *ptr == nullptr) {
    if (result == SHAKTI_SUCCESS) {
      return finish(result, "success");
    }
    return finish(result, backendOperationMessage("shaktiMalloc", *backend, result));
  }

  if (!shakti::registerAllocation(*ptr, backend->name())) {
    backend->free(*ptr);
    *ptr = nullptr;
    return finish(SHAKTI_ERROR_OUT_OF_MEMORY,
                  "shaktiMalloc failed: allocation registry could not record pointer");
  }

  return finish(SHAKTI_SUCCESS, "success");
}

ShaktiResult shaktiFree(void* ptr) {
  shakti::Backend* backend = shakti::selectedBackend();
  std::string detail;
  ShaktiResult backend_status = ensureUsableBackend(backend, &detail);
  if (backend_status != SHAKTI_SUCCESS) {
    return finish(backend_status, detail);
  }

  ShaktiResult owner_status = ensureAllocationOwnedBy(ptr, *backend, &detail);
  if (owner_status != SHAKTI_SUCCESS) {
    return finish(owner_status, detail);
  }

  ShaktiResult result = backend->free(ptr);
  if (result == SHAKTI_SUCCESS) {
    shakti::unregisterAllocation(ptr);
    return finish(SHAKTI_SUCCESS, "success");
  }

  return finish(result, backendOperationMessage("shaktiFree", *backend, result));
}

ShaktiResult shaktiMemcpy(void* dst, const void* src, size_t bytes, ShaktiMemcpyKind kind) {
  shakti::Backend* backend = shakti::selectedBackend();
  std::string detail;
  ShaktiResult backend_status = ensureUsableBackend(backend, &detail);
  if (backend_status != SHAKTI_SUCCESS) {
    return finish(backend_status, detail);
  }

  ShaktiResult owner_status = ensureMemcpyOwnership(dst, src, bytes, *backend, &detail);
  if (owner_status != SHAKTI_SUCCESS) {
    return finish(owner_status, detail);
  }

  ShaktiResult result = backend->memcpy(dst, src, bytes, kind);
  if (result == SHAKTI_SUCCESS) {
    return finish(SHAKTI_SUCCESS, "success");
  }

  return finish(result, backendOperationMessage("shaktiMemcpy", *backend, result));
}

ShaktiResult shaktiLaunchKernel(ShaktiKernelFn kernel, ShaktiDim3 grid_dim,
                                ShaktiDim3 block_dim, void* args,
                                size_t shared_memory_bytes) {
  shakti::Backend* backend = shakti::selectedBackend();
  std::string detail;
  ShaktiResult backend_status = ensureUsableBackend(backend, &detail);
  if (backend_status != SHAKTI_SUCCESS) {
    return finish(backend_status, detail);
  }

  ShaktiResult result =
      backend->launchKernel(kernel, grid_dim, block_dim, args, shared_memory_bytes);
  if (result == SHAKTI_SUCCESS) {
    return finish(SHAKTI_SUCCESS, "success");
  }

  return finish(result, backendOperationMessage("shaktiLaunchKernel", *backend, result));
}

ShaktiResult shaktiDeviceSynchronize(void) {
  shakti::Backend* backend = shakti::selectedBackend();
  std::string detail;
  ShaktiResult backend_status = ensureUsableBackend(backend, &detail);
  if (backend_status != SHAKTI_SUCCESS) {
    return finish(backend_status, detail);
  }

  ShaktiResult result = backend->deviceSynchronize();
  if (result == SHAKTI_SUCCESS) {
    return finish(SHAKTI_SUCCESS, "success");
  }

  return finish(result, backendOperationMessage("shaktiDeviceSynchronize", *backend, result));
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
    return finish(SHAKTI_ERROR_INVALID_VALUE,
                  "shaktiGetBackendInfo failed: info output pointer is null");
  }

  shakti::Backend* backend = shakti::backendAt(index);
  if (backend == nullptr) {
    return finish(SHAKTI_ERROR_INVALID_VALUE,
                  "shaktiGetBackendInfo failed: backend index is out of range");
  }

  fillBackendInfo(*backend, info);
  return finish(SHAKTI_SUCCESS, "success");
}

ShaktiResult shaktiGetSelectedBackendInfo(ShaktiBackendInfo* info) {
  if (info == nullptr) {
    return finish(SHAKTI_ERROR_INVALID_VALUE,
                  "shaktiGetSelectedBackendInfo failed: info output pointer is null");
  }

  shakti::Backend* backend = shakti::selectedBackend();
  if (backend == nullptr) {
    return finish(SHAKTI_ERROR_INVALID_VALUE,
                  "selected backend is unknown; check SHAKTI_BACKEND");
  }

  fillBackendInfo(*backend, info);
  return finish(SHAKTI_SUCCESS, "success");
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

const char* shaktiGetLastErrorMessage(void) {
  return last_error_message.c_str();
}

}  // extern "C"
