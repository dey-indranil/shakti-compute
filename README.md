# Shakti Compute

Shakti Compute is an experimental, open-source, vendor-neutral accelerated computing runtime.

The goal is to provide a CUDA-familiar programming model that can eventually target multiple backends, including CPU, CUDA, HIP/ROCm, Level Zero, and SPIR-V.

## Current Status

Experimental. v1.3 has CPU and mock GPU backends, backend selection, backend capability queries, CI, optional CUDA and HIP memory support, allocation ownership checks, CUDA/HIP availability diagnostics, and a minimal CPU launch abstraction.

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

CUDA memory support is optional and requires the CUDA Toolkit. HIP memory support
is optional and requires ROCm/HIP. Configure optional backends with:

```sh
cmake -S . -B build -DSHAKTI_ENABLE_CUDA=ON
cmake -S . -B build -DSHAKTI_ENABLE_HIP=ON
```

CUDA-enabled builds still require a usable CUDA device and driver before the
CUDA backend reports itself as available. HIP-enabled builds likewise require a
usable HIP device and runtime before the HIP backend reports itself as available.

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

`SHAKTI_BACKEND=cuda` and `SHAKTI_BACKEND=hip` are recognized backend names.
CUDA only performs memory operations when Shakti is built with
`SHAKTI_ENABLE_CUDA=ON`; HIP only performs memory operations when Shakti is built
with `SHAKTI_ENABLE_HIP=ON`. Both backends still return
`SHAKTI_ERROR_UNAVAILABLE` for kernel launch.
`SHAKTI_BACKEND=mock_gpu` is available for hardware-free backend dispatch and
memory testing.

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

For v1.3, successful memory behavior is implemented by the CPU and mock GPU
backends, by CUDA when built with `SHAKTI_ENABLE_CUDA=ON`, and by HIP when built
with `SHAKTI_ENABLE_HIP=ON`.
Shakti tracks allocations internally and rejects freeing or copying known Shakti
allocations through the wrong selected backend.
`shaktiLaunchKernel` runs a host function synchronously on the CPU backend and is
a launch-shape placeholder for future GPU work. Backend info APIs report whether
each backend supports memory, launch, streams, and events.

For a beginner-friendly walkthrough with examples, see `docs/api.md`.

## CI

GitHub Actions builds and tests the project on Ubuntu and macOS for every push to
`main` and every pull request.

## License

Shakti Compute is licensed under the Apache License, Version 2.0. See `LICENSE`.
