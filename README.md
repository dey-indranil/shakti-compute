# Shakti Compute

Shakti Compute is an experimental, open-source, vendor-neutral accelerated computing runtime.

The goal is to provide a CUDA-familiar programming model that can eventually target multiple backends, including CPU, CUDA, HIP/ROCm, Level Zero, and SPIR-V.

## Current Status

Experimental. v0.1 starts with a CPU backend and a minimal runtime API.

## Goals

- CUDA-familiar runtime API
- Backend-neutral design
- CPU fallback for debugging and testing
- Future support for CUDA, HIP, Level Zero, and SPIR-V
- Future MLIR-based compiler pipeline

## Non-Goals For v0.1

- Full CUDA compatibility
- Replacing cuDNN or cuBLAS
- Production GPU performance
- Supporting every CUDA API
- Shipping a compiler or kernel language

## Build

```sh
cmake -S . -B build
cmake --build build
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

## Public API

The initial C API lives in `include/shakti/runtime.h` and provides:

- `shaktiMalloc`
- `shaktiFree`
- `shaktiMemcpy`
- `shaktiDeviceSynchronize`
- `shaktiGetErrorString`

For v0.1, all runtime behavior is implemented by the CPU backend.

## CI

GitHub Actions builds and tests the project on Ubuntu and macOS for every push to
`main` and every pull request.
