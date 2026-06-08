#ifndef SHAKTI_RUNTIME_H_
#define SHAKTI_RUNTIME_H_

#include <stddef.h>

#include "shakti/types.h"

#ifdef __cplusplus
extern "C" {
#endif

ShaktiResult shaktiMalloc(void** ptr, size_t bytes);
ShaktiResult shaktiFree(void* ptr);
ShaktiResult shaktiMemcpy(void* dst, const void* src, size_t bytes, ShaktiMemcpyKind kind);
ShaktiResult shaktiLaunchKernel(ShaktiKernelFn kernel, ShaktiDim3 grid_dim,
                                ShaktiDim3 block_dim, void* args,
                                size_t shared_memory_bytes);
ShaktiResult shaktiDeviceSynchronize(void);
const char* shaktiGetBackendName(void);
int shaktiIsBackendAvailable(const char* name);
size_t shaktiGetBackendCount(void);
ShaktiResult shaktiGetBackendInfo(size_t index, ShaktiBackendInfo* info);
ShaktiResult shaktiGetSelectedBackendInfo(ShaktiBackendInfo* info);
const char* shaktiGetErrorString(ShaktiResult result);
const char* shaktiGetLastErrorMessage(void);

#ifdef __cplusplus
}
#endif

#endif
