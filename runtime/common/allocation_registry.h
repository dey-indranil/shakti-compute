#ifndef SHAKTI_RUNTIME_COMMON_ALLOCATION_REGISTRY_H_
#define SHAKTI_RUNTIME_COMMON_ALLOCATION_REGISTRY_H_

#include <stddef.h>

namespace shakti {

bool registerAllocation(void* ptr, const char* backend_name);
void unregisterAllocation(void* ptr);
const char* allocationOwner(void* ptr);

}  // namespace shakti

#endif
