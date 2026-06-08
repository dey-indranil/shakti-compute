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

}  // namespace

int main() {
  bool ok = true;

  setenv("SHAKTI_BACKEND", "cpu", 1);
  void* cpu_ptr = nullptr;
  ok = ok && expect(shaktiMalloc(&cpu_ptr, 16) == SHAKTI_SUCCESS,
                    "CPU allocation succeeds");

  setenv("SHAKTI_BACKEND", "mock_gpu", 1);
  ok = ok && expect(shaktiFree(cpu_ptr) == SHAKTI_ERROR_INVALID_VALUE,
                    "mock GPU cannot free a CPU allocation");

  setenv("SHAKTI_BACKEND", "cpu", 1);
  ok = ok && expect(shaktiFree(cpu_ptr) == SHAKTI_SUCCESS,
                    "CPU can free its own allocation after rejected free");

  setenv("SHAKTI_BACKEND", "mock_gpu", 1);
  int source[4] = {1, 2, 3, 4};
  int destination[4] = {0, 0, 0, 0};
  void* mock_ptr = nullptr;
  ok = ok && expect(shaktiMalloc(&mock_ptr, sizeof(source)) == SHAKTI_SUCCESS,
                    "mock GPU allocation succeeds");
  ok = ok && expect(shaktiMemcpy(mock_ptr, source, sizeof(source),
                                 SHAKTI_MEMCPY_HOST_TO_DEVICE) == SHAKTI_SUCCESS,
                    "mock GPU can copy into its own allocation");

  setenv("SHAKTI_BACKEND", "cpu", 1);
  ok = ok && expect(shaktiMemcpy(destination, mock_ptr, sizeof(destination),
                                 SHAKTI_MEMCPY_DEVICE_TO_HOST) ==
                        SHAKTI_ERROR_INVALID_VALUE,
                    "CPU cannot copy from a mock GPU allocation");

  setenv("SHAKTI_BACKEND", "mock_gpu", 1);
  ok = ok && expect(shaktiMemcpy(destination, mock_ptr, sizeof(destination),
                                 SHAKTI_MEMCPY_DEVICE_TO_HOST) == SHAKTI_SUCCESS,
                    "mock GPU can copy from its own allocation");
  ok = ok && expect(std::memcmp(source, destination, sizeof(source)) == 0,
                    "mock GPU copy preserves bytes after rejected CPU copy");
  ok = ok && expect(shaktiFree(mock_ptr) == SHAKTI_SUCCESS,
                    "mock GPU can free its own allocation");

  unsetenv("SHAKTI_BACKEND");

  std::cout << (ok ? "PASS" : "FAIL") << "\n";
  return ok ? 0 : 1;
}
