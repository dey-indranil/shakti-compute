# Programming Model

Shakti Compute v0.9 exposes a small C runtime API inspired by familiar accelerator runtimes.

The implemented memory backends are CPU and mock GPU. Both use ordinary host memory, and `shaktiMemcpy` performs checked byte copies for every `ShaktiMemcpyKind`.

Backend selection is intentionally minimal. If `SHAKTI_BACKEND` is unset or set to `cpu`, the CPU backend is used. If it is set to `mock_gpu`, Shakti selects a hardware-free backend that exercises non-CPU dispatch with host memory. If it is set to `cuda` or `hip`, Shakti selects a known backend skeleton that currently reports `SHAKTI_ERROR_UNAVAILABLE` for runtime operations. Unknown backend names return `SHAKTI_ERROR_INVALID_VALUE`.

Applications can call `shaktiGetBackendName` to inspect the selected backend name and `shaktiIsBackendAvailable` to check whether a known backend is usable. For richer introspection, `shaktiGetBackendCount`, `shaktiGetBackendInfo`, and `shaktiGetSelectedBackendInfo` expose backend availability, capability flags, and a status message.

The first launch abstraction is `shaktiLaunchKernel`. On the CPU backend, it synchronously invokes a host function with user args and a `ShaktiLaunchContext` containing grid dimensions, block dimensions, and shared-memory byte count. Mock GPU, CUDA, and HIP currently return `SHAKTI_ERROR_UNAVAILABLE` for launch.

Future versions may add GPU-aware launch mechanics, streams, events, and real GPU backend implementations. v0.x does not promise CUDA source compatibility.
