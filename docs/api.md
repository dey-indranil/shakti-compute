# Public API Guide

This guide explains the public Shakti Compute C API for people who are new to the project.

Shakti Compute is an experimental runtime for writing accelerator-style code against one small API. Today, the CPU backend is the only backend that actually runs work. CUDA and HIP are present as named skeleton backends so the project can grow toward multiple hardware targets without changing the public shape every milestone.

Use the umbrella header:

```cpp
#include <shakti/shakti.h>
```

## Mental Model

Think of Shakti Compute as three pieces:

- A memory API for allocating, freeing, and copying buffers.
- A backend API for asking which backend is selected and what it can do.
- A launch API for running a small kernel-shaped host function on the CPU backend.

For now, Shakti memory is ordinary host memory. `shaktiLaunchKernel` is synchronous on CPU. There are no streams, events, async copies, or real GPU kernels yet.

## Results And Errors

Most runtime functions return `ShaktiResult`.

```cpp
typedef enum ShaktiResult {
  SHAKTI_SUCCESS = 0,
  SHAKTI_ERROR_INVALID_VALUE = 1,
  SHAKTI_ERROR_OUT_OF_MEMORY = 2,
  SHAKTI_ERROR_UNAVAILABLE = 3,
  SHAKTI_ERROR_UNKNOWN = 999
} ShaktiResult;
```

Use `shaktiGetErrorString` for a readable message:

```cpp
ShaktiResult result = shaktiDeviceSynchronize();
if (result != SHAKTI_SUCCESS) {
  std::cerr << shaktiGetErrorString(result) << "\n";
}
```

Common meanings:

- `SHAKTI_SUCCESS`: the operation succeeded.
- `SHAKTI_ERROR_INVALID_VALUE`: an argument was invalid, such as a null output pointer or unknown backend.
- `SHAKTI_ERROR_OUT_OF_MEMORY`: allocation failed.
- `SHAKTI_ERROR_UNAVAILABLE`: the selected backend is known but cannot perform the requested operation yet.

## Memory API

### Allocate

```cpp
void* ptr = nullptr;
ShaktiResult result = shaktiMalloc(&ptr, 1024);
```

Rules:

- Pass the address of a `void*`.
- On success, `ptr` receives the allocation.
- `shaktiMalloc(&ptr, 0)` succeeds and stores `nullptr`.
- `shaktiMalloc(nullptr, bytes)` returns `SHAKTI_ERROR_INVALID_VALUE`.

### Free

```cpp
shaktiFree(ptr);
```

Rules:

- Free memory allocated by `shaktiMalloc`.
- `shaktiFree(nullptr)` is allowed.
- On the CPU backend, this maps to ordinary host allocation/free behavior.

### Copy

```cpp
shaktiMemcpy(dst, src, bytes, SHAKTI_MEMCPY_HOST_TO_HOST);
```

Memcpy kinds:

```cpp
SHAKTI_MEMCPY_HOST_TO_HOST
SHAKTI_MEMCPY_HOST_TO_DEVICE
SHAKTI_MEMCPY_DEVICE_TO_HOST
SHAKTI_MEMCPY_DEVICE_TO_DEVICE
```

In v1.1, CPU and mock GPU treat all valid memcpy kinds as checked byte copies. CUDA-enabled builds route copies through CUDA Runtime API calls. This keeps examples backend-shaped while real CUDA memory support grows.

## Backend Selection

Shakti selects the backend from the `SHAKTI_BACKEND` environment variable.

```sh
./build/examples/saxpy/saxpy
SHAKTI_BACKEND=cpu ./build/examples/saxpy/saxpy
SHAKTI_BACKEND=mock_gpu ./build/tests/backend_selection_smoke
SHAKTI_BACKEND=cuda ./build/examples/saxpy/saxpy
SHAKTI_BACKEND=hip ./build/examples/saxpy/saxpy
```

Current behavior:

- unset: selects `cpu`
- `cpu`: selects CPU
- `mock_gpu`: selects a hardware-free backend for dispatch and memory tests
- `cuda`: selects CUDA; unavailable by default, memory-capable when built with `SHAKTI_ENABLE_CUDA=ON`
- `hip`: selects HIP skeleton, currently unavailable
- anything else: unknown backend

Get the selected backend name:

```cpp
std::cout << shaktiGetBackendName() << "\n";
```

Check simple availability:

```cpp
if (shaktiIsBackendAvailable("cpu")) {
  std::cout << "CPU backend is usable\n";
}
```

## Backend Capability Queries

Use `ShaktiBackendInfo` when you need to list backends or explain what a backend supports.

```cpp
typedef struct ShaktiBackendInfo {
  const char* name;
  int available;
  int supports_memory;
  int supports_launch;
  int supports_streams;
  int supports_events;
  const char* status_message;
} ShaktiBackendInfo;
```

List all known backends:

```cpp
for (size_t i = 0; i < shaktiGetBackendCount(); ++i) {
  ShaktiBackendInfo info = {};
  if (shaktiGetBackendInfo(i, &info) == SHAKTI_SUCCESS) {
    std::cout << info.name << ": " << info.status_message << "\n";
  }
}
```

Inspect the selected backend:

```cpp
ShaktiBackendInfo selected = {};
if (shaktiGetSelectedBackendInfo(&selected) == SHAKTI_SUCCESS) {
  std::cout << "Selected backend: " << selected.name << "\n";
  std::cout << "Supports launch: " << selected.supports_launch << "\n";
}
```

In v1.1:

- CPU is available and supports memory and launch.
- Mock GPU is available and supports memory, but not launch.
- CUDA is known and supports memory only when built with `SHAKTI_ENABLE_CUDA=ON`.
- HIP is known but unavailable.
- No backend supports streams or events yet.

## Launch API

`shaktiLaunchKernel` is the first kernel-shaped API. It does not launch real GPU code yet. On CPU, it synchronously calls a host function.

Kernel function type:

```cpp
typedef void (*ShaktiKernelFn)(void* args, const ShaktiLaunchContext* context);
```

Launch context:

```cpp
typedef struct ShaktiLaunchContext {
  ShaktiDim3 grid_dim;
  ShaktiDim3 block_dim;
  size_t shared_memory_bytes;
} ShaktiLaunchContext;
```

Minimal example:

```cpp
struct AddOneArgs {
  int* values;
  int count;
};

void addOneKernel(void* raw_args, const ShaktiLaunchContext* context) {
  AddOneArgs* args = static_cast<AddOneArgs*>(raw_args);
  unsigned int threads = context->grid_dim.x * context->block_dim.x;

  for (unsigned int i = 0; i < threads && i < static_cast<unsigned int>(args->count); ++i) {
    args->values[i] += 1;
  }
}

int values[4] = {1, 2, 3, 4};
AddOneArgs args = {values, 4};
ShaktiDim3 grid = {2, 1, 1};
ShaktiDim3 block = {2, 1, 1};

ShaktiResult result = shaktiLaunchKernel(addOneKernel, grid, block, &args, 0);
```

Rules:

- `kernel` must not be null.
- Every grid and block dimension must be nonzero.
- `args` may be null if your kernel function can handle it.
- CPU launch is synchronous.
- CUDA and HIP skeletons return `SHAKTI_ERROR_UNAVAILABLE`.

## Mini Cookbook

### 1. Allocate, Copy, And Free

```cpp
float input[4] = {1.0f, 2.0f, 3.0f, 4.0f};
float output[4] = {};

void* storage = nullptr;
if (shaktiMalloc(&storage, sizeof(input)) != SHAKTI_SUCCESS) {
  return 1;
}

shaktiMemcpy(storage, input, sizeof(input), SHAKTI_MEMCPY_HOST_TO_DEVICE);
shaktiMemcpy(output, storage, sizeof(output), SHAKTI_MEMCPY_DEVICE_TO_HOST);
shaktiFree(storage);
```

### 2. Print All Backends

```cpp
for (size_t i = 0; i < shaktiGetBackendCount(); ++i) {
  ShaktiBackendInfo info = {};
  ShaktiResult result = shaktiGetBackendInfo(i, &info);
  if (result == SHAKTI_SUCCESS) {
    std::cout << info.name
              << " available=" << info.available
              << " memory=" << info.supports_memory
              << " launch=" << info.supports_launch
              << " status=\"" << info.status_message << "\"\n";
  }
}
```

### 3. Require A Backend With Launch Support

```cpp
ShaktiBackendInfo info = {};
if (shaktiGetSelectedBackendInfo(&info) != SHAKTI_SUCCESS || !info.supports_launch) {
  std::cerr << "Selected backend cannot launch kernels\n";
  return 1;
}
```

### 4. Run A CPU-Backed Kernel

```cpp
struct ScaleArgs {
  float* values;
  float factor;
  int count;
};

void scaleKernel(void* raw_args, const ShaktiLaunchContext* context) {
  ScaleArgs* args = static_cast<ScaleArgs*>(raw_args);
  unsigned int threads = context->grid_dim.x * context->block_dim.x;

  for (unsigned int i = 0; i < threads && i < static_cast<unsigned int>(args->count); ++i) {
    args->values[i] *= args->factor;
  }
}

float values[8] = {1, 2, 3, 4, 5, 6, 7, 8};
ScaleArgs args = {values, 10.0f, 8};
ShaktiDim3 grid = {2, 1, 1};
ShaktiDim3 block = {4, 1, 1};

shaktiLaunchKernel(scaleKernel, grid, block, &args, 0);
```

### 5. Handle Unavailable CUDA Or HIP

```cpp
ShaktiResult result = shaktiMalloc(&ptr, 1024);
if (result == SHAKTI_ERROR_UNAVAILABLE) {
  ShaktiBackendInfo info = {};
  if (shaktiGetSelectedBackendInfo(&info) == SHAKTI_SUCCESS) {
    std::cerr << info.name << ": " << info.status_message << "\n";
  }
}
```

This is what happens today with `SHAKTI_BACKEND=cuda` or `SHAKTI_BACKEND=hip`.

### 6. Test Non-CPU Dispatch Without Hardware

```sh
SHAKTI_BACKEND=mock_gpu ./build/tests/backend_selection_smoke
```

The mock GPU backend uses host memory internally, but it is selected through the
same backend registry as real backends. This lets the project test memory
dispatch through a non-CPU backend before CUDA or HIP hardware is available.

### 7. Test CUDA Memory On A CUDA Machine

```sh
cmake -S . -B build-cuda -DSHAKTI_ENABLE_CUDA=ON
cmake --build build-cuda
SHAKTI_BACKEND=cuda ./build-cuda/tests/cuda_memory_smoke
```

This optional smoke test allocates CUDA memory, copies host data to the device,
copies it back, synchronizes, and frees the allocation. It passes without CUDA in
the default build by confirming that CUDA is known but unavailable. In a
CUDA-enabled build, the CUDA backend reports available only when a usable CUDA
device and driver are present. If CUDA is unavailable, inspect
`ShaktiBackendInfo.status_message` for the reason.

## What Shakti Does Not Do Yet

Shakti Compute does not yet:

- compile kernels
- run real CUDA kernels
- run real HIP kernels
- provide streams or events
- provide async copies
- manage real device memory
- promise CUDA source compatibility

Those are future milestones. The current API exists to make the runtime shape testable before real GPU backend work begins.
