#ifndef HSA_QUEUE_PROFILING_H
#define HSA_QUEUE_PROFILING_H

#include <map>
#include <hsa_ext_profiler.h>
#include "util.h"

uint64_t init_timestamp;

std::map<uint64_t, const char*> symbol_names;
std::map<uint64_t, uint64_t> kernel_object_symbols;

void gather_kernel_times();
hsa_status_t aql_callback(const hsa_aql_trace_t *aql_trace, void *user_arg);

#endif
