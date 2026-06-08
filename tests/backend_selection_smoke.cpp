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
  ShaktiBackendInfo info = {};

  ok = ok && expect(shaktiGetBackendCount() == 4, "backend count is 4");
  ok = ok && expect(shaktiGetBackendInfo(0, &info) == SHAKTI_SUCCESS,
                    "CPU backend info is available");
  ok = ok && expect(std::strcmp(info.name, "cpu") == 0, "backend 0 is CPU");
  ok = ok && expect(info.available == 1, "CPU backend info reports available");
  ok = ok && expect(info.supports_memory == 1, "CPU backend supports memory");
  ok = ok && expect(info.supports_launch == 1, "CPU backend supports launch");
  ok = ok && expect(info.supports_streams == 0, "CPU backend does not support streams");
  ok = ok && expect(info.supports_events == 0, "CPU backend does not support events");
  ok = ok && expect(info.status_message != nullptr, "CPU backend has status message");

  ok = ok && expect(shaktiGetBackendInfo(1, &info) == SHAKTI_SUCCESS,
                    "mock GPU backend info is available");
  ok = ok && expect(std::strcmp(info.name, "mock_gpu") == 0, "backend 1 is mock GPU");
  ok = ok && expect(info.available == 1, "mock GPU backend info reports available");
  ok = ok && expect(info.supports_memory == 1, "mock GPU backend supports memory");
  ok = ok && expect(info.supports_launch == 0, "mock GPU backend does not support launch");
  ok = ok && expect(info.supports_streams == 0, "mock GPU backend does not support streams");
  ok = ok && expect(info.supports_events == 0, "mock GPU backend does not support events");
  ok = ok && expect(info.status_message != nullptr, "mock GPU backend has status message");

  ok = ok && expect(shaktiGetBackendInfo(2, &info) == SHAKTI_SUCCESS,
                    "CUDA backend info is available");
  ok = ok && expect(std::strcmp(info.name, "cuda") == 0, "backend 2 is CUDA");
  const int cuda_available = info.available;
  ok = ok && expect(info.supports_memory == cuda_available,
                    "CUDA memory support matches availability");
  ok = ok && expect(info.supports_launch == 0, "CUDA skeleton does not support launch");
  ok = ok && expect(info.status_message != nullptr, "CUDA backend has status message");

  ok = ok && expect(shaktiGetBackendInfo(3, &info) == SHAKTI_SUCCESS,
                    "HIP backend info is available");
  ok = ok && expect(std::strcmp(info.name, "hip") == 0, "backend 3 is HIP");
  const int hip_available = info.available;
  ok = ok && expect(info.supports_memory == hip_available,
                    "HIP memory support matches availability");
  ok = ok && expect(info.supports_launch == 0, "HIP launch remains unavailable");
  ok = ok && expect(info.status_message != nullptr, "HIP backend has status message");

  ok = ok && expect(shaktiGetBackendInfo(4, &info) == SHAKTI_ERROR_INVALID_VALUE,
                    "invalid backend index is rejected");
  ok = ok && expect(shaktiGetBackendInfo(0, nullptr) == SHAKTI_ERROR_INVALID_VALUE,
                    "null backend info output is rejected");

  unsetenv("SHAKTI_BACKEND");
  ok = ok && expect(allocationWorks(), "unset SHAKTI_BACKEND selects CPU");
  ok = ok && expect(std::strcmp(shaktiGetBackendName(), "cpu") == 0,
                    "unset SHAKTI_BACKEND reports CPU backend");
  ok = ok && expect(shaktiGetSelectedBackendInfo(&info) == SHAKTI_SUCCESS,
                    "selected CPU backend info is available");
  ok = ok && expect(std::strcmp(info.name, "cpu") == 0,
                    "selected backend info reports CPU");

  setenv("SHAKTI_BACKEND", "cpu", 1);
  ok = ok && expect(allocationWorks(), "SHAKTI_BACKEND=cpu selects CPU");
  ok = ok && expect(std::strcmp(shaktiGetBackendName(), "cpu") == 0,
                    "SHAKTI_BACKEND=cpu reports CPU backend");

  ok = ok && expect(shaktiIsBackendAvailable("cpu") == 1, "CPU backend is available");
  ok = ok && expect(shaktiIsBackendAvailable("mock_gpu") == 1,
                    "mock GPU backend is available");
  ok = ok && expect(shaktiIsBackendAvailable("cuda") == cuda_available,
                    "CUDA availability matches backend info");
  ok = ok && expect(shaktiIsBackendAvailable("hip") == hip_available,
                    "HIP availability matches backend info");
  ok = ok && expect(shaktiIsBackendAvailable("bogus") == 0,
                    "unknown backend is unavailable");

  setenv("SHAKTI_BACKEND", "mock_gpu", 1);
  int source[4] = {1, 2, 3, 4};
  int destination[4] = {0, 0, 0, 0};
  void* mock_ptr = nullptr;
  ok = ok && expect(std::strcmp(shaktiGetBackendName(), "mock_gpu") == 0,
                    "SHAKTI_BACKEND=mock_gpu reports mock GPU backend");
  ok = ok && expect(shaktiGetSelectedBackendInfo(&info) == SHAKTI_SUCCESS,
                    "selected mock GPU backend info is available");
  ok = ok && expect(std::strcmp(info.name, "mock_gpu") == 0,
                    "selected backend info reports mock GPU");
  ok = ok && expect(shaktiMalloc(&mock_ptr, sizeof(source)) == SHAKTI_SUCCESS,
                    "mock GPU backend allocates memory");
  ok = ok && expect(shaktiMemcpy(mock_ptr, source, sizeof(source),
                                 SHAKTI_MEMCPY_HOST_TO_DEVICE) == SHAKTI_SUCCESS,
                    "mock GPU backend copies host to device");
  ok = ok && expect(shaktiMemcpy(destination, mock_ptr, sizeof(destination),
                                 SHAKTI_MEMCPY_DEVICE_TO_HOST) == SHAKTI_SUCCESS,
                    "mock GPU backend copies device to host");
  ok = ok && expect(std::memcmp(source, destination, sizeof(source)) == 0,
                    "mock GPU backend round-trips bytes");
  ok = ok && expect(shaktiDeviceSynchronize() == SHAKTI_SUCCESS,
                    "mock GPU backend synchronizes");
  ShaktiDim3 grid = {1, 1, 1};
  ShaktiDim3 block = {1, 1, 1};
  ok = ok && expect(shaktiLaunchKernel(emptyKernel, grid, block, nullptr, 0) ==
                        SHAKTI_ERROR_UNAVAILABLE,
                    "mock GPU backend launch is unavailable");
  ok = ok && expect(shaktiFree(mock_ptr) == SHAKTI_SUCCESS,
                    "mock GPU backend frees memory");

  setenv("SHAKTI_BACKEND", "cuda", 1);
  void* ptr = reinterpret_cast<void*>(0x1);
  ok = ok && expect(std::strcmp(shaktiGetBackendName(), "cuda") == 0,
                    "SHAKTI_BACKEND=cuda reports CUDA backend");
  ok = ok && expect(shaktiGetSelectedBackendInfo(&info) == SHAKTI_SUCCESS,
                    "selected CUDA backend info is available");
  ok = ok && expect(std::strcmp(info.name, "cuda") == 0,
                    "selected backend info reports CUDA");
  ShaktiResult cuda_malloc_result = shaktiMalloc(&ptr, 16);
  if (cuda_available) {
    ok = ok && expect(cuda_malloc_result == SHAKTI_SUCCESS,
                      "CUDA backend allocates memory when available");
    ok = ok && expect(shaktiFree(ptr) == SHAKTI_SUCCESS,
                      "CUDA backend frees memory when available");
  } else {
    ok = ok && expect(cuda_malloc_result == SHAKTI_ERROR_UNAVAILABLE,
                      "CUDA skeleton returns unavailable");
    ok = ok && expect(ptr == nullptr, "CUDA skeleton clears allocation output");
  }
  ok = ok && expect(shaktiLaunchKernel(emptyKernel, grid, block, nullptr, 0) ==
                        SHAKTI_ERROR_UNAVAILABLE,
                    "CUDA launch remains unavailable");

  setenv("SHAKTI_BACKEND", "hip", 1);
  ptr = reinterpret_cast<void*>(0x1);
  ok = ok && expect(std::strcmp(shaktiGetBackendName(), "hip") == 0,
                    "SHAKTI_BACKEND=hip reports HIP backend");
  ok = ok && expect(shaktiGetSelectedBackendInfo(&info) == SHAKTI_SUCCESS,
                    "selected HIP backend info is available");
  ok = ok && expect(std::strcmp(info.name, "hip") == 0,
                    "selected backend info reports HIP");
  ShaktiResult hip_malloc_result = shaktiMalloc(&ptr, 16);
  if (hip_available) {
    ok = ok && expect(hip_malloc_result == SHAKTI_SUCCESS,
                      "HIP backend allocates memory when available");
    ok = ok && expect(shaktiFree(ptr) == SHAKTI_SUCCESS,
                      "HIP backend frees memory when available");
  } else {
    ok = ok && expect(hip_malloc_result == SHAKTI_ERROR_UNAVAILABLE,
                      "HIP backend returns unavailable when not available");
    ok = ok && expect(ptr == nullptr, "HIP backend clears allocation output");
  }
  ok = ok && expect(shaktiLaunchKernel(emptyKernel, grid, block, nullptr, 0) ==
                        SHAKTI_ERROR_UNAVAILABLE,
                    "HIP launch remains unavailable");

  setenv("SHAKTI_BACKEND", "bogus", 1);
  ptr = reinterpret_cast<void*>(0x1);
  ok = ok && expect(std::strcmp(shaktiGetBackendName(), "unknown") == 0,
                    "unknown backend reports unknown");
  ok = ok && expect(shaktiGetSelectedBackendInfo(&info) == SHAKTI_ERROR_INVALID_VALUE,
                    "unknown selected backend info is rejected");
  ok = ok && expect(shaktiMalloc(&ptr, 16) == SHAKTI_ERROR_INVALID_VALUE,
                    "unknown backend returns invalid value");
  ok = ok && expect(ptr == nullptr, "unsupported backend clears allocation output");

  unsetenv("SHAKTI_BACKEND");

  std::cout << (ok ? "PASS" : "FAIL") << "\n";
  return ok ? 0 : 1;
}
