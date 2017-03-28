#ifndef HSA_PERF_COUNTERS_H
#define HSA_PERF_COUNTERS_H

#include <pthread.h>
#include <map>
#include <amd_hsa_tools_interfaces.h>
#include "util.h"

pthread_mutex_t mutex;
gpa_uint32 session_id;
std::map<gpa_uint32, uint64_t> session_kernel_objects;

void pre_dispatch_callback(const hsa_dispatch_callback_t* dispatch_params, void* user_args);
void post_dispatch_callback(const hsa_dispatch_callback_t* dispatch_params, void* user_args);
void close_context(gpa_uint32 session_id);
void read_gpa_counters(gpa_uint32 session_id);

#endif
