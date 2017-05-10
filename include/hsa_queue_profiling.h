#ifndef HSA_QUEUE_PROFILING_H
#define HSA_QUEUE_PROFILING_H

#include <hsa_ext_profiler.h>
#include "util.h"

hsa_status_t aql_callback(const hsa_aql_trace_t *aql_trace, void *user_arg);

#endif
