# Shakti Compute Contributor Instructions

Shakti Compute is an experimental open-source accelerated computing runtime.

## Design Principles

1. Keep v0 small and buildable.
2. Prefer boring, portable C++17.
3. Every new public runtime API must have a CPU backend implementation.
4. Every runtime function must have a smoke test.
5. Do not claim full CUDA compatibility.
6. Public APIs use the `shakti` prefix.
7. Internal C++ symbols use the `shakti::` namespace.
8. Avoid vendor-specific code in public headers.
9. Keep examples simple and runnable on machines without GPUs.

## Current Milestone

Build a minimal CPU backend with:

- `shaktiMalloc`
- `shaktiFree`
- `shaktiMemcpy`
- `shaktiDeviceSynchronize`
- `shaktiGetErrorString`
- SAXPY example
- smoke tests
