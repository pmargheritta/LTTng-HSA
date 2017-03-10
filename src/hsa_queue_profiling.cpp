#include "hsa_queue_profiling.h"

void gather_kernel_times()
{
    size_t count = hsa_ext_tools_get_kernel_times(0, NULL);
    if (count > 0) {
        hsa_profiler_kernel_time_t* records = new hsa_profiler_kernel_time_t[count];
        count = hsa_ext_tools_get_kernel_times(count, records);

        hsa_profiler_kernel_time_t record;
        const char* name;
        for (int i = 0; i < count; i++) {
            record = records[i];
            name = symbol_names[kernel_object_symbols[record.kernel]];
            tracepoint(hsa_runtime, kernel_start_nm, record.kernel, name, record.agent.handle, record.queue->id, record.time.start);
            tracepoint(hsa_runtime, kernel_end_nm, record.kernel, name, record.agent.handle, record.queue->id, record.time.end);
        }

        delete[] records;
    }
}

hsa_status_t aql_callback(const hsa_aql_trace_t *aql_trace, void *user_arg)
{
    switch (aql_trace->type) {
        case HSA_PACKET_TYPE_KERNEL_DISPATCH:
            /*tracepoint(hsa_runtime, aql_kernel_dispatch_packet_submitted,
  	             aql_trace->packet_id, aql_trace->agent.handle, aql_trace->queue->id,
                 ((hsa_kernel_dispatch_packet_t*) aql_trace->packet)->kernel_object);*/
            break;
        default:
            break;
    }

    return HSA_STATUS_SUCCESS;
}

hsa_status_t hsa_queue_create(hsa_agent_t agent, uint32_t size, hsa_queue_type_t type,
    void (*callback)(hsa_status_t status, hsa_queue_t *source,
    void *data),
    void *data, uint32_t private_segment_size,
    uint32_t group_segment_size, hsa_queue_t **queue)
{
    decltype(hsa_ext_tools_queue_create_profiled) *orig = hsa_ext_tools_queue_create_profiled;

    hsa_status_t retval = orig(agent, size, type, callback, data, private_segment_size, group_segment_size, queue);

    uint64_t timestamp;
    hsa_system_get_info(HSA_SYSTEM_INFO_TIMESTAMP, &timestamp);
    tracepoint(hsa_runtime, queue_created, agent.handle, (*queue)->id, timestamp);

    return retval;
}

hsa_status_t hsa_executable_get_symbol(
      hsa_executable_t executable,
      const char *module_name,
      const char *symbol_name,
      hsa_agent_t agent,
      int32_t call_convention,
      hsa_executable_symbol_t *symbol)
{
    decltype(hsa_executable_get_symbol) *orig = (decltype(hsa_executable_get_symbol)*) dlsym(RTLD_NEXT, "hsa_executable_get_symbol");
    hsa_status_t retval = orig(executable, module_name, symbol_name, agent, call_convention, symbol);
    symbol_names[symbol->handle] = symbol_name;
    return retval;
}

hsa_status_t hsa_executable_symbol_get_info(
      hsa_executable_symbol_t executable_symbol,
      hsa_executable_symbol_info_t attribute,
      void *value)
{
    decltype(hsa_executable_symbol_get_info) *orig = (decltype(hsa_executable_symbol_get_info)*) dlsym(RTLD_NEXT, "hsa_executable_symbol_get_info");
    hsa_status_t retval = orig(executable_symbol, attribute, value);
    if (attribute == HSA_EXECUTABLE_SYMBOL_INFO_KERNEL_OBJECT) {
        kernel_object_symbols[*((uint64_t*) value)] = executable_symbol.handle;
    }
    return retval;
}

hsa_status_t hsa_queue_destroy(hsa_queue_t *queue)
{
    decltype(hsa_queue_destroy) *orig = (decltype(hsa_queue_destroy)*) dlsym(RTLD_NEXT, "hsa_queue_destroy");

    gather_kernel_times();

    uint64_t queue_id = queue->id;
    hsa_status_t retval = orig(queue);

    uint64_t timestamp;
    hsa_system_get_info(HSA_SYSTEM_INFO_TIMESTAMP, &timestamp);
    tracepoint(hsa_runtime, queue_destroyed, queue_id, timestamp);

    return retval;
}
