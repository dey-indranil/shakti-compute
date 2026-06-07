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

## Later

- Real GPU kernels
- Python launcher bindings
- Microbenchmark harness
- MLIR research branch
