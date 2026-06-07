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
