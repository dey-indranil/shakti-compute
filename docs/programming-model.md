# Programming Model

Shakti Compute v1.3 exposes a small C runtime API inspired by familiar accelerator runtimes.

The always-available memory backends are CPU and mock GPU. Both use ordinary host memory, and `shaktiMemcpy` performs checked byte copies for every `ShaktiMemcpyKind`. CUDA can also provide real device memory operations when Shakti is configured with `SHAKTI_ENABLE_CUDA=ON` and the CUDA Toolkit is available. HIP can provide real device memory operations when Shakti is configured with `SHAKTI_ENABLE_HIP=ON` and ROCm/HIP is available.

Backend selection is intentionally minimal. If `SHAKTI_BACKEND` is unset or set to `cpu`, the CPU backend is used. If it is set to `mock_gpu`, Shakti selects a hardware-free backend that exercises non-CPU dispatch with host memory. If it is set to `cuda`, Shakti selects the CUDA backend, which is unavailable in default builds and memory-capable in CUDA-enabled builds. If it is set to `hip`, Shakti selects the HIP backend, which is unavailable in default builds and memory-capable in HIP-enabled builds. Unknown backend names return `SHAKTI_ERROR_INVALID_VALUE`.

Applications can call `shaktiGetBackendName` to inspect the selected backend name and `shaktiIsBackendAvailable` to check whether a known backend is usable. For richer introspection, `shaktiGetBackendCount`, `shaktiGetBackendInfo`, and `shaktiGetSelectedBackendInfo` expose backend availability, capability flags, and a status message.

Shakti tracks allocations created by `shaktiMalloc` and records the backend that created each pointer. If code tries to free or copy a known Shakti allocation through a different selected backend, the runtime returns `SHAKTI_ERROR_INVALID_VALUE` before calling into the backend. Plain host pointers that Shakti did not allocate are still accepted as host-side copy operands.

The first launch abstraction is `shaktiLaunchKernel`. On the CPU backend, it synchronously invokes a host function with user args and a `ShaktiLaunchContext` containing grid dimensions, block dimensions, and shared-memory byte count. Mock GPU, CUDA, and HIP currently return `SHAKTI_ERROR_UNAVAILABLE` for launch.

Future versions may add GPU-aware launch mechanics, streams, events, and real GPU backend implementations. v0.x does not promise CUDA source compatibility.
