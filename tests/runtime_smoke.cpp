#include "shakti/shakti.h"

#include <cstring>
#include <iostream>

namespace {

struct IncrementArgs {
  int* values;
  int count;
};

void incrementKernel(void* raw_args, const ShaktiLaunchContext* context) {
  IncrementArgs* args = static_cast<IncrementArgs*>(raw_args);
  const unsigned int threads = context->grid_dim.x * context->block_dim.x;
  for (unsigned int i = 0; i < threads && i < static_cast<unsigned int>(args->count);
       ++i) {
    args->values[i] += 1;
  }
}

bool expect(bool condition, const char* message) {
  if (condition) {
    return true;
  }

  std::cerr << "FAIL: " << message << "\n";
  return false;
}

bool expectSuccess(ShaktiResult result, const char* operation) {
  if (result == SHAKTI_SUCCESS) {
    return true;
  }

  std::cerr << "FAIL: " << operation << " returned " << shaktiGetErrorString(result)
            << "\n";
  return false;
}

}  // namespace

int main() {
  bool ok = true;

  void* ptr = nullptr;
  ok = ok && expectSuccess(shaktiMalloc(&ptr, 64), "shaktiMalloc");
  ok = ok && expect(ptr != nullptr, "shaktiMalloc returns non-null for nonzero bytes");
  ok = ok && expectSuccess(shaktiFree(ptr), "shaktiFree");

  void* zero_ptr = reinterpret_cast<void*>(0x1);
  ok = ok && expectSuccess(shaktiMalloc(&zero_ptr, 0), "shaktiMalloc zero bytes");
  ok = ok && expect(zero_ptr == nullptr, "zero-byte shaktiMalloc stores nullptr");
  ok = ok && expectSuccess(shaktiFree(nullptr), "shaktiFree nullptr");

  int src[4] = {1, 2, 3, 4};
  int dst[4] = {0, 0, 0, 0};
  ok = ok && expectSuccess(
                 shaktiMemcpy(dst, src, sizeof(src), SHAKTI_MEMCPY_HOST_TO_HOST),
                 "shaktiMemcpy");
  ok = ok && expect(std::memcmp(dst, src, sizeof(src)) == 0, "shaktiMemcpy copies bytes");

  ok = ok && expect(shaktiMalloc(nullptr, 16) == SHAKTI_ERROR_INVALID_VALUE,
                    "shaktiMalloc rejects null output pointer");
  ok = ok && expect(shaktiMemcpy(nullptr, src, sizeof(src), SHAKTI_MEMCPY_HOST_TO_HOST) ==
                        SHAKTI_ERROR_INVALID_VALUE,
                    "shaktiMemcpy rejects null destination for nonzero bytes");
  ok = ok && expect(shaktiMemcpy(dst, nullptr, sizeof(dst), SHAKTI_MEMCPY_HOST_TO_HOST) ==
                        SHAKTI_ERROR_INVALID_VALUE,
                    "shaktiMemcpy rejects null source for nonzero bytes");
  ok = ok && expect(shaktiMemcpy(nullptr, nullptr, 0, SHAKTI_MEMCPY_HOST_TO_HOST) ==
                        SHAKTI_SUCCESS,
                    "shaktiMemcpy accepts null pointers for zero bytes");
  ok = ok && expect(shaktiMemcpy(dst, src, sizeof(src), static_cast<ShaktiMemcpyKind>(99)) ==
                        SHAKTI_ERROR_INVALID_VALUE,
                    "shaktiMemcpy rejects invalid memcpy kind");

  int launch_values[4] = {1, 2, 3, 4};
  IncrementArgs launch_args = {launch_values, 4};
  ShaktiDim3 grid = {2, 1, 1};
  ShaktiDim3 block = {2, 1, 1};
  ok = ok && expectSuccess(shaktiLaunchKernel(incrementKernel, grid, block, &launch_args, 0),
                           "shaktiLaunchKernel");
  ok = ok && expect(launch_values[0] == 2 && launch_values[1] == 3 &&
                        launch_values[2] == 4 && launch_values[3] == 5,
                    "shaktiLaunchKernel invokes CPU kernel");

  ShaktiDim3 zero_grid = {0, 1, 1};
  ok = ok && expect(shaktiLaunchKernel(incrementKernel, zero_grid, block, &launch_args, 0) ==
                        SHAKTI_ERROR_INVALID_VALUE,
                    "shaktiLaunchKernel rejects zero grid dimension");
  ok = ok && expect(shaktiLaunchKernel(nullptr, grid, block, &launch_args, 0) ==
                        SHAKTI_ERROR_INVALID_VALUE,
                    "shaktiLaunchKernel rejects null kernel");

  ok = ok && expectSuccess(shaktiDeviceSynchronize(), "shaktiDeviceSynchronize");

  ok = ok && expect(shaktiGetErrorString(SHAKTI_SUCCESS) != nullptr,
                    "success error string is non-null");
  ok = ok && expect(shaktiGetErrorString(SHAKTI_ERROR_INVALID_VALUE) != nullptr,
                    "invalid value error string is non-null");
  ok = ok && expect(shaktiGetErrorString(SHAKTI_ERROR_OUT_OF_MEMORY) != nullptr,
                    "out of memory error string is non-null");
  ok = ok && expect(shaktiGetErrorString(SHAKTI_ERROR_UNAVAILABLE) != nullptr,
                    "unavailable error string is non-null");
  ok = ok && expect(shaktiGetErrorString(SHAKTI_ERROR_UNKNOWN) != nullptr,
                    "unknown error string is non-null");
  ok = ok && expect(shaktiGetErrorString(static_cast<ShaktiResult>(-1)) != nullptr,
                    "unrecognized error string is non-null");

  std::cout << (ok ? "PASS" : "FAIL") << "\n";
  return ok ? 0 : 1;
}
