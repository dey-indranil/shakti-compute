#ifndef SHAKTI_RUNTIME_COMMON_BACKEND_REGISTRY_H_
#define SHAKTI_RUNTIME_COMMON_BACKEND_REGISTRY_H_

#include <stddef.h>

#include "common/backend.h"

namespace shakti {

size_t backendCount();
Backend* backendAt(size_t index);
Backend* backendByName(const char* name);
Backend* selectedBackend();

}  // namespace shakti

#endif
