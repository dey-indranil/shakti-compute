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

## v1.1: CUDA Diagnostics Or HIP Memory

- Improve CUDA unavailable/error reporting
- Or add optional HIP malloc/free/memcpy/sync
- Keep default CI hardware-free

## Later

- Real GPU kernels
- Python launcher bindings
- Microbenchmark harness
- MLIR research branch
