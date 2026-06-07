#include "shakti/shakti.h"

#include <cstdlib>
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

}  // namespace

int main() {
  bool ok = true;

  unsetenv("SHAKTI_BACKEND");
  ok = ok && expect(allocationWorks(), "unset SHAKTI_BACKEND selects CPU");

  setenv("SHAKTI_BACKEND", "cpu", 1);
  ok = ok && expect(allocationWorks(), "SHAKTI_BACKEND=cpu selects CPU");

  setenv("SHAKTI_BACKEND", "cuda", 1);
  void* ptr = reinterpret_cast<void*>(0x1);
  ok = ok && expect(shaktiMalloc(&ptr, 16) == SHAKTI_ERROR_INVALID_VALUE,
                    "unsupported backend returns invalid value");
  ok = ok && expect(ptr == nullptr, "unsupported backend clears allocation output");

  unsetenv("SHAKTI_BACKEND");

  std::cout << (ok ? "PASS" : "FAIL") << "\n";
  return ok ? 0 : 1;
}
