# Programming Model

Shakti Compute v0.4 exposes a small C runtime API inspired by familiar accelerator runtimes.

The only implemented backend is CPU. Memory allocated with `shaktiMalloc` is ordinary host memory, and `shaktiMemcpy` performs checked byte copies for every `ShaktiMemcpyKind`.

Backend selection is intentionally minimal. If `SHAKTI_BACKEND` is unset or set to `cpu`, the CPU backend is used. If it is set to `cuda` or `hip`, Shakti selects a known backend skeleton that currently reports `SHAKTI_ERROR_UNAVAILABLE` for runtime operations. Unknown backend names return `SHAKTI_ERROR_INVALID_VALUE`.

Applications can call `shaktiGetBackendName` to inspect the selected backend name and `shaktiIsBackendAvailable` to check whether a known backend is usable.

Future versions may add CUDA-like kernel launch concepts, streams, events, and real GPU backend implementations. v0.x does not promise CUDA source compatibility.
