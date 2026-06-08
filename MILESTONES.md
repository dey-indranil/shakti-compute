# Shakti Compute Milestone Log

This file records what each milestone achieved, what was intentionally left out, and the recommended next step. Keep entries short and factual so future contributors can understand the project history quickly.

## v0.1: CPU Runtime Seed

Status: implemented locally

Achieved:

- Created a C++17/CMake project skeleton.
- Added the `shakti_runtime` static library target.
- Added public headers under `include/shakti/`.
- Implemented the initial public C runtime API:
  - `shaktiMalloc`
  - `shaktiFree`
  - `shaktiMemcpy`
  - `shaktiDeviceSynchronize`
  - `shaktiGetErrorString`
- Implemented a CPU-only backend where Shakti allocations are host allocations and all valid memcpy kinds use checked byte copies.
- Added the `saxpy` example target.
- Added the `runtime_smoke` test target.
- Added starter documentation:
  - `README.md`
  - `AGENTS.md`
  - `docs/vision.md`
  - `docs/roadmap.md`
  - `docs/programming-model.md`

Verified:

- `cmake -S . -B build`
- `cmake --build build`
- `./build/examples/saxpy/saxpy`
- `./build/tests/runtime_smoke`

Deferred:

- Git repository initialization and commits.
- CI.
- Backend interface abstraction.
- CUDA and HIP backend skeletons.
- Streams, events, async copies, and kernel launch APIs.
- MLIR, compiler work, and CUDA source compatibility.

Recommended next milestone:

- v0.2 should introduce a backend interface, backend selection plumbing, and CPU backend registration while preserving the existing public API.

## v0.2: Backend Shape

Status: implemented locally

Achieved:

- Added a private `shakti::Backend` interface for runtime backends.
- Split the runtime into common public API dispatch and a CPU backend implementation.
- Added CPU backend registration through `shakti::cpuBackend()`.
- Added `SHAKTI_BACKEND` selection plumbing:
  - unset selects CPU
  - `SHAKTI_BACKEND=cpu` selects CPU
  - unsupported backend names return `SHAKTI_ERROR_INVALID_VALUE`
- Preserved the existing public C API.
- Added a `backend_selection_smoke` test target.
- Updated documentation for backend selection and the roadmap.

Verified:

- `cmake --build build`
- `./build/examples/saxpy/saxpy`
- `SHAKTI_BACKEND=cpu ./build/examples/saxpy/saxpy`
- `./build/tests/runtime_smoke`
- `./build/tests/backend_selection_smoke`

Deferred:

- Public backend query APIs.
- CUDA and HIP backend skeleton files.
- Backend-specific allocator metadata.
- Streams, events, async copies, and kernel launch APIs.

Recommended next milestone:

- v0.3 should add CI before introducing CUDA/HIP skeletons.

## v0.3: CI Baseline

Status: implemented locally

Achieved:

- Added CTest registration for runtime smoke tests.
- Added a GitHub Actions CI workflow.
- CI builds on Ubuntu and macOS.
- CI runs `ctest`, the SAXPY example, and the explicit CPU backend SAXPY path.
- Updated project version to `0.3.0`.
- Updated README and roadmap docs for CI.

Verified:

- `cmake -S . -B build`
- `cmake --build build`
- `ctest --test-dir build --output-on-failure`
- `./build/examples/saxpy/saxpy`
- `SHAKTI_BACKEND=cpu ./build/examples/saxpy/saxpy`

Deferred:

- CUDA and HIP backend skeleton files.
- Public backend query APIs.
- Kernel launch abstraction.
- Compiler or MLIR work.

Recommended next milestone:

- v0.4 should add CUDA and HIP backend skeletons behind the existing backend interface, without changing the public runtime API.

## v0.4: Backend Skeletons

Status: implemented locally

Achieved:

- Added `SHAKTI_ERROR_UNAVAILABLE`.
- Added public backend query APIs:
  - `shaktiGetBackendName`
  - `shaktiIsBackendAvailable`
- Added CUDA and HIP backend skeletons behind the private backend interface.
- Added `SHAKTI_ENABLE_CUDA` and `SHAKTI_ENABLE_HIP` CMake flags.
- Extended backend selection:
  - unset selects CPU
  - `SHAKTI_BACKEND=cpu` selects CPU
  - `SHAKTI_BACKEND=cuda` selects the CUDA skeleton
  - `SHAKTI_BACKEND=hip` selects the HIP skeleton
  - unknown names return `SHAKTI_ERROR_INVALID_VALUE`
- Kept CPU as the only backend with successful runtime operations.
- Extended smoke tests for backend names, availability, and unavailable skeleton behavior.
- Updated README, programming model, roadmap, and project version.

Verified:

- `cmake -S . -B build`
- `cmake --build build`
- `ctest --test-dir build --output-on-failure`
- `./build/examples/saxpy/saxpy`
- `SHAKTI_BACKEND=cpu ./build/examples/saxpy/saxpy`

Deferred:

- Real CUDA runtime calls.
- Real HIP runtime calls.
- Backend-specific allocator metadata.
- Kernel launch abstraction.
- Streams, events, and async copies.

Recommended next milestone:

- v0.5 should add a minimal kernel launch abstraction with a CPU launch path before implementing real GPU launches.

## v0.5: Kernel Launch Abstraction

Status: implemented locally

Achieved:

- Added launch-related public types:
  - `ShaktiDim3`
  - `ShaktiLaunchContext`
  - `ShaktiKernelFn`
- Added `shaktiLaunchKernel`.
- Added `launchKernel` to the private backend interface.
- Implemented CPU launch as a synchronous host function call.
- Kept CUDA and HIP launch behavior explicit as `SHAKTI_ERROR_UNAVAILABLE`.
- Updated the SAXPY example to run through `shaktiLaunchKernel`.
- Added smoke tests for launch success, invalid launch inputs, and unavailable skeleton launches.
- Updated README, programming model, roadmap, and project version.

Verified:

- `cmake -S . -B build`
- `cmake --build build`
- `ctest --test-dir build --output-on-failure`
- `./build/examples/saxpy/saxpy`
- `SHAKTI_BACKEND=cpu ./build/examples/saxpy/saxpy`

Deferred:

- Real GPU launch implementation.
- Streams and async launch behavior.
- Kernel argument packing beyond a raw `void*`.
- Backend capability metadata.

Recommended next milestone:

- v0.6 should refine backend/capability queries and diagnostics before adding real CUDA or HIP runtime calls.

## v0.6: Backend Query Refinement

Status: implemented locally

Achieved:

- Added `ShaktiBackendInfo`.
- Added public backend introspection APIs:
  - `shaktiGetBackendCount`
  - `shaktiGetBackendInfo`
  - `shaktiGetSelectedBackendInfo`
- Added backend capability fields:
  - availability
  - memory support
  - launch support
  - stream support
  - event support
  - status message
- Extended the private backend interface with capability metadata.
- Reported CPU as available with memory and launch support.
- Reported CUDA and HIP skeletons as unavailable with no memory, launch, stream, or event support.
- Added smoke tests for backend count, backend info, selected backend info, invalid indexes, and null output pointers.
- Updated README, programming model, roadmap, and project version.

Verified:

- `cmake -S . -B build`
- `cmake --build build`
- `ctest --test-dir build --output-on-failure`
- `./build/examples/saxpy/saxpy`
- `SHAKTI_BACKEND=cpu ./build/examples/saxpy/saxpy`

Deferred:

- Real CUDA runtime calls.
- Real HIP runtime calls.
- Public backend lookup by name.
- Last-error message API.
- Streams and events.

Recommended next milestone:

- v0.7 should clean up backend lookup/registry internals before adding real CUDA memory operations.

## v0.7: Public API Documentation

Status: implemented locally

Achieved:

- Added `docs/api.md`.
- Documented what Shakti Compute currently does as a product/runtime.
- Explained result codes and error handling.
- Documented memory allocation, free, and copy APIs.
- Documented backend selection with `SHAKTI_BACKEND`.
- Documented backend capability APIs.
- Documented the CPU-backed launch API.
- Added a mini cookbook with examples for:
  - allocate/copy/free
  - listing backends
  - requiring launch support
  - running a CPU-backed kernel
  - handling unavailable CUDA/HIP skeletons
- Linked the API guide from README.

Verified:

- Documentation-only change.

Deferred:

- Generated API reference.
- Doxygen or hosted docs.
- Real CUDA/HIP API examples.

Recommended next milestone:

- v0.8 should clean up backend lookup/registry internals before adding real CUDA memory operations.

## v0.8: Backend Registry Cleanup

Status: implemented locally

Achieved:

- Added private backend registry helpers:
  - `backendCount`
  - `backendAt`
  - `backendByName`
  - `selectedBackend`
- Centralized backend order as CPU, CUDA, HIP.
- Removed hardcoded backend lookup branches from public runtime dispatch.
- Preserved the existing public API and behavior.
- Updated roadmap numbering after the public API documentation milestone.

Verified:

- `cmake -S . -B build`
- `cmake --build build`
- `ctest --test-dir build --output-on-failure`
- `./build/examples/saxpy/saxpy`
- `SHAKTI_BACKEND=cpu ./build/examples/saxpy/saxpy`

Deferred:

- Real CUDA memory operations.
- Real HIP memory operations.
- Public backend lookup by name.
- Dynamic backend registration.

Recommended next milestone:

- v0.9 should start the CUDA memory seed behind `SHAKTI_ENABLE_CUDA`, while keeping CPU and CI behavior unchanged on machines without CUDA.

## v0.9: Mock GPU Backend

Status: implemented locally

Achieved:

- Added a reusable private `HostMemoryBackend` base for host-memory-backed backends.
- Refactored CPU memory operations through `HostMemoryBackend`.
- Added a `mock_gpu` backend.
- Registered `mock_gpu` as a known backend after CPU and before CUDA/HIP.
- Implemented mock GPU malloc/free/memcpy/sync with host memory.
- Kept mock GPU launch unavailable.
- Added smoke tests proving a non-CPU backend can allocate, copy, synchronize, report capabilities, and reject launch without GPU hardware.
- Updated README, programming model, API guide, roadmap, and project version.

Verified:

- `cmake -S . -B build`
- `cmake --build build`
- `ctest --test-dir build --output-on-failure`
- `./build/examples/saxpy/saxpy`
- `SHAKTI_BACKEND=cpu ./build/examples/saxpy/saxpy`

Deferred:

- Real CUDA memory operations.
- Real HIP memory operations.
- Mock GPU launch support.
- Dynamic backend registration.

Recommended next milestone:

- v1.0 should start the CUDA memory seed behind `SHAKTI_ENABLE_CUDA`, while keeping CPU, mock GPU, and CI behavior unchanged on machines without CUDA.

## v1.0: CUDA Memory Seed

Status: implemented locally

Achieved:

- Added optional CUDA Toolkit discovery when `SHAKTI_ENABLE_CUDA=ON`.
- Linked `CUDA::cudart` only for CUDA-enabled builds.
- Implemented CUDA backend memory operations behind the CUDA flag:
  - `shaktiMalloc`
  - `shaktiFree`
  - `shaktiMemcpy`
  - `shaktiDeviceSynchronize`
- Mapped common CUDA runtime errors to `ShaktiResult`.
- Kept CUDA launch unavailable.
- Added `cuda_memory_smoke`, which passes in default builds and exercises CUDA memory when CUDA is enabled.
- Kept default CPU/mock GPU CI behavior hardware-free.
- Updated README, programming model, API guide, roadmap, and project version.

Verified:

- `cmake -S . -B build`
- `cmake --build build`
- `ctest --test-dir build --output-on-failure`
- `./build/examples/saxpy/saxpy`
- `SHAKTI_BACKEND=cpu ./build/examples/saxpy/saxpy`
- `SHAKTI_BACKEND=mock_gpu ./build/tests/backend_selection_smoke`

Deferred:

- CUDA kernel launch.
- CUDA streams and events.
- CUDA allocator metadata.
- HIP memory operations.

Recommended next milestone:

- v1.1 should add optional HIP memory support or improve CUDA diagnostics depending on available contributor hardware.

## v1.1: CUDA Diagnostics

Status: implemented locally

Achieved:

- Improved CUDA backend status messages:
  - CUDA not built into Shakti
  - CUDA runtime found no devices
  - CUDA driver missing or insufficient
  - CUDA runtime error string for other device-count failures
- Kept CUDA availability tied to a usable CUDA device and driver.
- Extended `cuda_memory_smoke` to validate CUDA status messages and capability consistency in unavailable builds.
- Updated README, programming model, API guide, roadmap, and project version.

Verified:

- `cmake -S . -B build`
- `cmake --build build`
- `ctest --test-dir build --output-on-failure`
- `./build/examples/saxpy/saxpy`
- `SHAKTI_BACKEND=cpu ./build/examples/saxpy/saxpy`
- `SHAKTI_BACKEND=mock_gpu ./build/tests/backend_selection_smoke`

Deferred:

- CUDA kernel launch.
- CUDA streams and events.
- Per-call last-error details.
- HIP memory operations.

Recommended next milestone:

- v1.2 should add optional HIP memory support behind `SHAKTI_ENABLE_HIP`, using the CUDA memory seed as the pattern.
