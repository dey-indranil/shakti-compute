#include "common/allocation_registry.h"

#include <mutex>
#include <new>
#include <unordered_map>

namespace shakti {

namespace {

std::mutex& registryMutex() {
  static std::mutex mutex;
  return mutex;
}

std::unordered_map<void*, const char*>& registry() {
  static std::unordered_map<void*, const char*> allocations;
  return allocations;
}

}  // namespace

bool registerAllocation(void* ptr, const char* backend_name) {
  if (ptr == nullptr || backend_name == nullptr) {
    return true;
  }

  try {
    std::lock_guard<std::mutex> lock(registryMutex());
    registry()[ptr] = backend_name;
  } catch (const std::bad_alloc&) {
    return false;
  }

  return true;
}

void unregisterAllocation(void* ptr) {
  if (ptr == nullptr) {
    return;
  }

  std::lock_guard<std::mutex> lock(registryMutex());
  registry().erase(ptr);
}

const char* allocationOwner(void* ptr) {
  if (ptr == nullptr) {
    return nullptr;
  }

  std::lock_guard<std::mutex> lock(registryMutex());
  auto found = registry().find(ptr);
  if (found == registry().end()) {
    return nullptr;
  }

  return found->second;
}

}  // namespace shakti
