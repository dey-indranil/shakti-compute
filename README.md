# Shakti Compute

Shakti Compute is an experimental, open-source, vendor-neutral accelerated computing runtime.

The goal is to provide a CUDA-familiar programming model that can eventually target multiple backends, including CPU, CUDA, HIP/ROCm, Level Zero, and SPIR-V.

## Current Status

Experimental. v0.6 has a CPU backend, backend selection, backend capability queries, CI, CUDA/HIP backend skeletons, and a minimal CPU launch abstraction.

## Goals

- CUDA-familiar runtime API
- Backend-neutral design
- CPU fallback for debugging and testing
- Future support for CUDA, HIP, Level Zero, and SPIR-V
- Future MLIR-based compiler pipeline

## Non-Goals For v0.x

- Full CUDA compatibility
- Replacing cuDNN or cuBLAS
- Production GPU performance
- Supporting every CUDA API
- Shipping a compiler or kernel language
- Launching real GPU kernels

## Build

```sh
cmake -S . -B build
cmake --build build
```

CUDA and HIP backend skeletons are unavailable stubs. Future real backend work can
use these reserved CMake flags:

```sh
cmake -S . -B build -DSHAKTI_ENABLE_CUDA=ON
cmake -S . -B build -DSHAKTI_ENABLE_HIP=ON
```

## Test

```sh
ctest --test-dir build --output-on-failure
```

## Run Examples

```sh
./build/examples/saxpy/saxpy
./build/tests/runtime_smoke
./build/tests/backend_selection_smoke
```

All commands should print `PASS`.

The runtime currently selects the CPU backend by default. You can also request it
explicitly:

```sh
SHAKTI_BACKEND=cpu ./build/examples/saxpy/saxpy
```

`SHAKTI_BACKEND=cuda` and `SHAKTI_BACKEND=hip` are recognized backend names, but
they currently return `SHAKTI_ERROR_UNAVAILABLE` for runtime operations.

## Public API

The initial C API lives in `include/shakti/runtime.h` and provides:

- `shaktiMalloc`
- `shaktiFree`
- `shaktiMemcpy`
- `shaktiLaunchKernel`
- `shaktiDeviceSynchronize`
- `shaktiGetBackendName`
- `shaktiIsBackendAvailable`
- `shaktiGetBackendCount`
- `shaktiGetBackendInfo`
- `shaktiGetSelectedBackendInfo`
- `shaktiGetErrorString`

For v0.6, successful runtime behavior is implemented by the CPU backend. CUDA and
HIP exist as explicit skeleton backends only. `shaktiLaunchKernel` runs a host
function synchronously on the CPU backend and is a launch-shape placeholder for
future GPU work. Backend info APIs report whether each backend supports memory,
launch, streams, and events.

For a beginner-friendly walkthrough with examples, see `docs/api.md`.

## CI

GitHub Actions builds and tests the project on Ubuntu and macOS for every push to
`main` and every pull request.
