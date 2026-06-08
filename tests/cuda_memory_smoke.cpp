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
  setenv("SHAKTI_BACKEND", "cuda", 1);

  ShaktiBackendInfo info = {};
  ShaktiResult info_result = shaktiGetSelectedBackendInfo(&info);
  if (info_result != SHAKTI_SUCCESS) {
    std::cerr << "FAIL: CUDA backend should be known\n";
    return 1;
  }

  bool ok = true;
  ok = ok && expect(info.status_message != nullptr,
                    "CUDA backend should provide a status message");

  if (!info.available) {
    ok = ok && expect(info.supports_memory == 0,
                      "unavailable CUDA backend should not report memory support");
    ok = ok && expect(shaktiIsBackendAvailable("cuda") == 0,
                      "unavailable CUDA backend should report unavailable");
    std::cout << (ok ? "PASS" : "FAIL") << "\n";
    unsetenv("SHAKTI_BACKEND");
    return ok ? 0 : 1;
  }

  int source[4] = {10, 20, 30, 40};
  int destination[4] = {0, 0, 0, 0};
  void* device_ptr = nullptr;

  ok = ok && expect(info.supports_memory == 1, "CUDA backend reports memory support");
  ok = ok && expect(info.supports_launch == 0, "CUDA backend launch remains unavailable");
  ok = ok && expect(shaktiMalloc(&device_ptr, sizeof(source)) == SHAKTI_SUCCESS,
                    "CUDA backend allocates memory");
  ok = ok && expect(shaktiMemcpy(device_ptr, source, sizeof(source),
                                 SHAKTI_MEMCPY_HOST_TO_DEVICE) == SHAKTI_SUCCESS,
                    "CUDA backend copies host to device");
  ok = ok && expect(shaktiMemcpy(destination, device_ptr, sizeof(destination),
                                 SHAKTI_MEMCPY_DEVICE_TO_HOST) == SHAKTI_SUCCESS,
                    "CUDA backend copies device to host");
  ok = ok && expect(shaktiDeviceSynchronize() == SHAKTI_SUCCESS,
                    "CUDA backend synchronizes");
  ok = ok && expect(std::memcmp(source, destination, sizeof(source)) == 0,
                    "CUDA backend round-trips bytes");
  ok = ok && expect(shaktiFree(device_ptr) == SHAKTI_SUCCESS,
                    "CUDA backend frees memory");

  unsetenv("SHAKTI_BACKEND");

  std::cout << (ok ? "PASS" : "FAIL") << "\n";
  return ok ? 0 : 1;
}
