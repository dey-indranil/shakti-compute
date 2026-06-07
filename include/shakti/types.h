#ifndef SHAKTI_TYPES_H_
#define SHAKTI_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef enum ShaktiResult {
  SHAKTI_SUCCESS = 0,
  SHAKTI_ERROR_INVALID_VALUE = 1,
  SHAKTI_ERROR_OUT_OF_MEMORY = 2,
  SHAKTI_ERROR_UNAVAILABLE = 3,
  SHAKTI_ERROR_UNKNOWN = 999
} ShaktiResult;

typedef enum ShaktiMemcpyKind {
  SHAKTI_MEMCPY_HOST_TO_HOST = 0,
  SHAKTI_MEMCPY_HOST_TO_DEVICE = 1,
  SHAKTI_MEMCPY_DEVICE_TO_HOST = 2,
  SHAKTI_MEMCPY_DEVICE_TO_DEVICE = 3
} ShaktiMemcpyKind;

typedef struct ShaktiDim3 {
  unsigned int x;
  unsigned int y;
  unsigned int z;
} ShaktiDim3;

typedef struct ShaktiLaunchContext {
  ShaktiDim3 grid_dim;
  ShaktiDim3 block_dim;
  size_t shared_memory_bytes;
} ShaktiLaunchContext;

typedef void (*ShaktiKernelFn)(void* args, const ShaktiLaunchContext* context);

typedef struct ShaktiBackendInfo {
  const char* name;
  int available;
  int supports_memory;
  int supports_launch;
  int supports_streams;
  int supports_events;
  const char* status_message;
} ShaktiBackendInfo;

#ifdef __cplusplus
}
#endif

#endif
