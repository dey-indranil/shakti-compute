#include "common/backend_registry.h"

#include <cstdlib>
#include <cstring>

namespace shakti {

namespace {

constexpr size_t kBackendCount = 4;

bool isEmpty(const char* value) {
  return value == nullptr || std::strcmp(value, "") == 0;
}

Backend* backends[kBackendCount] = {
    &cpuBackend(),
    &mockBackend(),
    &cudaBackend(),
    &hipBackend()
};

}  // namespace

size_t backendCount() {
  return kBackendCount;
}

Backend* backendAt(size_t index) {
  if (index >= backendCount()) {
    return nullptr;
  }

  return backends[index];
}

Backend* backendByName(const char* name) {
  if (isEmpty(name)) {
    return nullptr;
  }

  for (size_t i = 0; i < backendCount(); ++i) {
    Backend* backend = backendAt(i);
    if (std::strcmp(name, backend->name()) == 0) {
      return backend;
    }
  }

  return nullptr;
}

Backend* selectedBackend() {
  const char* requested_backend = std::getenv("SHAKTI_BACKEND");
  if (isEmpty(requested_backend)) {
    return &cpuBackend();
  }

  return backendByName(requested_backend);
}

}  // namespace shakti
