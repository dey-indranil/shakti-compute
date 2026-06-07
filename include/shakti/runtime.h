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
ShaktiResult shaktiDeviceSynchronize(void);
const char* shaktiGetErrorString(ShaktiResult result);

#ifdef __cplusplus
}
#endif

#endif
