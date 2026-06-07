#include "shakti/shakti.h"

#include <cmath>
#include <iostream>
#include <vector>

namespace {

struct SaxpyArgs {
  float* y;
  const float* x;
  float a;
  int n;
};

void saxpyKernel(void* raw_args, const ShaktiLaunchContext* context) {
  SaxpyArgs* args = static_cast<SaxpyArgs*>(raw_args);
  const unsigned int threads_per_grid =
      context->grid_dim.x * context->block_dim.x;

  for (unsigned int i = 0; i < threads_per_grid && i < static_cast<unsigned int>(args->n);
       ++i) {
    args->y[i] = args->a * args->x[i] + args->y[i];
  }
}

bool check(ShaktiResult result, const char* operation) {
  if (result == SHAKTI_SUCCESS) {
    return true;
  }

  std::cerr << operation << " failed: " << shaktiGetErrorString(result) << "\n";
  return false;
}

}  // namespace

int main() {
  constexpr int n = 1024;
  constexpr float a = 2.0f;

  std::vector<float> host_x(n);
  std::vector<float> host_y(n);
  std::vector<float> result_y(n);

  for (int i = 0; i < n; ++i) {
    host_x[i] = static_cast<float>(i);
    host_y[i] = static_cast<float>(n - i);
  }

  void* x_storage = nullptr;
  void* y_storage = nullptr;

  if (!check(shaktiMalloc(&x_storage, n * sizeof(float)), "shaktiMalloc(x)") ||
      !check(shaktiMalloc(&y_storage, n * sizeof(float)), "shaktiMalloc(y)")) {
    shaktiFree(x_storage);
    shaktiFree(y_storage);
    std::cout << "FAIL\n";
    return 1;
  }

  float* device_x = static_cast<float*>(x_storage);
  float* device_y = static_cast<float*>(y_storage);

  bool ok = true;
  ok = ok && check(shaktiMemcpy(device_x, host_x.data(), n * sizeof(float),
                                SHAKTI_MEMCPY_HOST_TO_DEVICE),
                   "shaktiMemcpy(x)");
  ok = ok && check(shaktiMemcpy(device_y, host_y.data(), n * sizeof(float),
                                SHAKTI_MEMCPY_HOST_TO_DEVICE),
                   "shaktiMemcpy(y)");

  if (ok) {
    SaxpyArgs args = {device_y, device_x, a, n};
    ShaktiDim3 grid = {static_cast<unsigned int>((n + 255) / 256), 1, 1};
    ShaktiDim3 block = {256, 1, 1};
    ok = ok && check(shaktiLaunchKernel(saxpyKernel, grid, block, &args, 0),
                     "shaktiLaunchKernel");
    ok = ok && check(shaktiDeviceSynchronize(), "shaktiDeviceSynchronize");
    ok = ok && check(shaktiMemcpy(result_y.data(), device_y, n * sizeof(float),
                                  SHAKTI_MEMCPY_DEVICE_TO_HOST),
                     "shaktiMemcpy(result)");
  }

  for (int i = 0; i < n && ok; ++i) {
    const float expected = a * host_x[i] + host_y[i];
    if (std::fabs(result_y[i] - expected) > 1e-5f) {
      std::cerr << "mismatch at " << i << ": got " << result_y[i] << ", expected "
                << expected << "\n";
      ok = false;
    }
  }

  shaktiFree(x_storage);
  shaktiFree(y_storage);

  std::cout << (ok ? "PASS" : "FAIL") << "\n";
  return ok ? 0 : 1;
}
