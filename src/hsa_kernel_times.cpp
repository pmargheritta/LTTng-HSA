#include "hsa_kernel_times.h"

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
            tracepoint(hsa_runtime, kernel_start_nm, record.kernel, name, record.agent.handle, record.queue->id, record.time.start - init_timestamp);
            tracepoint(hsa_runtime, kernel_end_nm, record.kernel, name, record.agent.handle, record.queue->id, record.time.end - init_timestamp);
        }

        delete[] records;
    }
}

hsa_status_t hsa_init()
{
    return my_hsa_init();
}

hsa_status_t hsa_queue_create(hsa_agent_t agent, uint32_t size, hsa_queue_type_t type,
    void (*callback)(hsa_status_t status, hsa_queue_t *source,
    void *data),
    void *data, uint32_t private_segment_size,
    uint32_t group_segment_size, hsa_queue_t **queue)
{
    decltype(hsa_ext_tools_queue_create_profiled) *orig = hsa_ext_tools_queue_create_profiled;
    hsa_status_t retval = orig(agent, size, type, callback, data, private_segment_size, group_segment_size, queue);
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
    return my_hsa_executable_get_symbol(executable, module_name, symbol_name, agent, call_convention, symbol);
}

hsa_status_t hsa_executable_symbol_get_info(
      hsa_executable_symbol_t executable_symbol,
      hsa_executable_symbol_info_t attribute,
      void *value)
{
    return my_hsa_executable_symbol_get_info(executable_symbol, attribute, value);
}

hsa_status_t hsa_queue_destroy(hsa_queue_t *queue)
{
    decltype(hsa_queue_destroy) *orig = (decltype(hsa_queue_destroy)*) dlsym(RTLD_NEXT, "hsa_queue_destroy");
    gather_kernel_times();
    hsa_status_t retval = orig(queue);
    return retval;
}
