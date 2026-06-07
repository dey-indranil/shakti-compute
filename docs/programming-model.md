# Programming Model

Shakti Compute v0.1 exposes a small C runtime API inspired by familiar accelerator runtimes.

The only implemented backend is CPU. Memory allocated with `shaktiMalloc` is ordinary host memory, and `shaktiMemcpy` performs checked byte copies for every `ShaktiMemcpyKind`.

Backend selection is intentionally minimal. If `SHAKTI_BACKEND` is unset or set to `cpu`, the CPU backend is used. Other backend names are reserved for future milestones and currently return an invalid value error from runtime calls.

Future versions may add CUDA-like kernel launch concepts, streams, events, and backend selection. v0.1 does not promise CUDA source compatibility.
