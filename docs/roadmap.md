# Roadmap

## v0.1: CPU Runtime Seed

- C++17 runtime API
- CPU backend
- SAXPY example
- Runtime smoke tests
- Basic contributor documentation

## v0.2: Backend Shape

- Private backend interface
- Runtime dispatch through selected backend
- `SHAKTI_BACKEND=cpu` selection
- CPU backend registration

## v0.3: CI Baseline

- GitHub Actions workflow
- Ubuntu and macOS build matrix
- CTest registration for smoke tests
- SAXPY example checks in CI

## v0.4: Backend Skeletons

- Public backend query APIs
- CUDA backend skeleton
- HIP backend skeleton
- CMake backend flags
- Unavailable-backend behavior tests

## v0.5: Kernel Launch Abstraction

- Minimal launch API
- CPU launch path for debug and tests

## v0.6: Backend Query Refinement

- Backend capability flags
- More detailed unavailable diagnostics
- Public backend list API

## v0.7: Public API Documentation

- Beginner-friendly API guide
- Cookbook examples
- Current limitations

## v0.8: Backend Registry Cleanup

- Shared backend lookup helpers
- Cleaner backend selection path
- Capability-driven tests

## v0.9: Mock GPU Backend

- Hardware-free non-CPU backend
- Host-memory-backed mock malloc/free/memcpy/sync
- Mock backend capability tests

## v1.0: CUDA Memory Seed

- Optional CUDA configure path
- CUDA malloc/free/memcpy/sync
- Manual CUDA smoke test

## v1.1: CUDA Diagnostics

- Improve CUDA unavailable/error reporting
- Validate CUDA status messages in smoke tests
- Keep default CI hardware-free

## v1.2: HIP Memory Seed

- Optional HIP configure path
- HIP malloc/free/memcpy/sync
- Manual HIP smoke test

## v1.3: Memory Ownership Diagnostics

- Track which backend created each Shakti allocation
- Reject obvious cross-backend frees and copies with clearer errors
- Keep CPU/mock/CUDA/HIP memory behavior covered by smoke tests

## v1.4: Backend Error Diagnostics

- Add a last-error or detailed diagnostic API
- Preserve backend-specific failure messages after runtime calls
- Improve user-facing explanations for unavailable GPU backends

## v1.5: Allocation Metadata

- Track allocation sizes
- Reject obvious out-of-range copy sizes
- Prepare metadata for future profiler/debug tooling

## Later

- Real GPU kernels
- Python launcher bindings
- Microbenchmark harness
- MLIR research branch
