#include "shakti/shakti.h"

#include <cstdlib>
#include <cstring>
#include <iostream>

namespace {

bool expect(bool condition, const char* message) {
  if (condition) {
    return true;
  }

  std::cerr << "FAIL: " << message << "\n";
  return false;
}

bool allocationWorks() {
  void* ptr = nullptr;
  const ShaktiResult result = shaktiMalloc(&ptr, 16);
  shaktiFree(ptr);
  return result == SHAKTI_SUCCESS && ptr != nullptr;
}

void emptyKernel(void* /*args*/, const ShaktiLaunchContext* /*context*/) {}

}  // namespace

int main() {
  bool ok = true;

  unsetenv("SHAKTI_BACKEND");
  ok = ok && expect(allocationWorks(), "unset SHAKTI_BACKEND selects CPU");
  ok = ok && expect(std::strcmp(shaktiGetBackendName(), "cpu") == 0,
                    "unset SHAKTI_BACKEND reports CPU backend");

  setenv("SHAKTI_BACKEND", "cpu", 1);
  ok = ok && expect(allocationWorks(), "SHAKTI_BACKEND=cpu selects CPU");
  ok = ok && expect(std::strcmp(shaktiGetBackendName(), "cpu") == 0,
                    "SHAKTI_BACKEND=cpu reports CPU backend");

  ok = ok && expect(shaktiIsBackendAvailable("cpu") == 1, "CPU backend is available");
  ok = ok && expect(shaktiIsBackendAvailable("cuda") == 0,
                    "CUDA backend skeleton is unavailable by default");
  ok = ok && expect(shaktiIsBackendAvailable("hip") == 0,
                    "HIP backend skeleton is unavailable by default");
  ok = ok && expect(shaktiIsBackendAvailable("bogus") == 0,
                    "unknown backend is unavailable");

  setenv("SHAKTI_BACKEND", "cuda", 1);
  void* ptr = reinterpret_cast<void*>(0x1);
  ok = ok && expect(std::strcmp(shaktiGetBackendName(), "cuda") == 0,
                    "SHAKTI_BACKEND=cuda reports CUDA backend");
  ok = ok && expect(shaktiMalloc(&ptr, 16) == SHAKTI_ERROR_UNAVAILABLE,
                    "CUDA skeleton returns unavailable");
  ShaktiDim3 grid = {1, 1, 1};
  ShaktiDim3 block = {1, 1, 1};
  ok = ok && expect(shaktiLaunchKernel(emptyKernel, grid, block, nullptr, 0) ==
                        SHAKTI_ERROR_UNAVAILABLE,
                    "CUDA skeleton launch returns unavailable");
  ok = ok && expect(ptr == nullptr, "CUDA skeleton clears allocation output");

  setenv("SHAKTI_BACKEND", "hip", 1);
  ptr = reinterpret_cast<void*>(0x1);
  ok = ok && expect(std::strcmp(shaktiGetBackendName(), "hip") == 0,
                    "SHAKTI_BACKEND=hip reports HIP backend");
  ok = ok && expect(shaktiMalloc(&ptr, 16) == SHAKTI_ERROR_UNAVAILABLE,
                    "HIP skeleton returns unavailable");
  ok = ok && expect(shaktiLaunchKernel(emptyKernel, grid, block, nullptr, 0) ==
                        SHAKTI_ERROR_UNAVAILABLE,
                    "HIP skeleton launch returns unavailable");
  ok = ok && expect(ptr == nullptr, "HIP skeleton clears allocation output");

  setenv("SHAKTI_BACKEND", "bogus", 1);
  ptr = reinterpret_cast<void*>(0x1);
  ok = ok && expect(std::strcmp(shaktiGetBackendName(), "unknown") == 0,
                    "unknown backend reports unknown");
  ok = ok && expect(shaktiMalloc(&ptr, 16) == SHAKTI_ERROR_INVALID_VALUE,
                    "unknown backend returns invalid value");
  ok = ok && expect(ptr == nullptr, "unsupported backend clears allocation output");

  unsetenv("SHAKTI_BACKEND");

  std::cout << (ok ? "PASS" : "FAIL") << "\n";
  return ok ? 0 : 1;
}
