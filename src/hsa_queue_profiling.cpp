#include "hsa_queue_profiling.h"

hsa_status_t aql_callback(const hsa_aql_trace_t *aql_trace, void *user_arg)
{
    switch (aql_trace->type) {
        case HSA_PACKET_TYPE_VENDOR_SPECIFIC:
            tracepoint(hsa_runtime, aql_packet_submitted, aql_trace->packet_id,
                "vendor_specific", aql_trace->agent.handle, aql_trace->queue->id);
        case HSA_PACKET_TYPE_INVALID:
            tracepoint(hsa_runtime, aql_packet_submitted, aql_trace->packet_id,
                "invalid", aql_trace->agent.handle, aql_trace->queue->id);
        case HSA_PACKET_TYPE_BARRIER_AND:
            tracepoint(hsa_runtime, aql_packet_submitted, aql_trace->packet_id,
                "barrier_and", aql_trace->agent.handle, aql_trace->queue->id);
        case HSA_PACKET_TYPE_AGENT_DISPATCH:
            tracepoint(hsa_runtime, aql_packet_submitted, aql_trace->packet_id,
                "agent_dispatch", aql_trace->agent.handle, aql_trace->queue->id);
        case HSA_PACKET_TYPE_BARRIER_OR:
            tracepoint(hsa_runtime, aql_packet_submitted, aql_trace->packet_id,
                "barrier_or", aql_trace->agent.handle, aql_trace->queue->id);
        case HSA_PACKET_TYPE_KERNEL_DISPATCH: {
            uint64_t kernel_object = ((hsa_kernel_dispatch_packet_t*) aql_trace->packet)->kernel_object;
            const char* name = symbol_names[kernel_object_symbols[kernel_object]];
            tracepoint(hsa_runtime, aql_kernel_dispatch_packet_submitted,
                aql_trace->packet_id, aql_trace->agent.handle, aql_trace->queue->id, kernel_object, name);
        } break;
        default:
            break;
    }

    return HSA_STATUS_SUCCESS;
}

hsa_status_t hsa_init()
{
    decltype(hsa_init) *orig = (decltype(hsa_init)*) dlsym(RTLD_NEXT, "hsa_init");
    hsa_status_t retval = orig();
    tracepoint(hsa_runtime, runtime_initialized);
    return retval;
}

hsa_status_t hsa_shut_down()
{
    decltype(hsa_shut_down) *orig = (decltype(hsa_shut_down)*) dlsym(RTLD_NEXT, "hsa_shut_down");
    hsa_status_t retval = orig();
    tracepoint(hsa_runtime, runtime_shut_down);
    return retval;
}

hsa_status_t hsa_queue_create(hsa_agent_t agent, uint32_t size, hsa_queue_type_t type,
    void (*callback)(hsa_status_t status, hsa_queue_t *source,
    void *data),
    void *data, uint32_t private_segment_size,
    uint32_t group_segment_size, hsa_queue_t **queue)
{
    decltype(hsa_ext_tools_queue_create_profiled) *orig = hsa_ext_tools_queue_create_profiled;
    hsa_status_t retval = orig(agent, size, type, callback, data, private_segment_size, group_segment_size, queue);

    tracepoint(hsa_runtime, queue_created, agent.handle, (*queue)->id);

    hsa_ext_tools_register_aql_trace_callback(*queue, NULL, aql_callback);

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

    uint64_t queue_id = queue->id;
    hsa_status_t retval = orig(queue);

    tracepoint(hsa_runtime, queue_destroyed, queue_id);

    return retval;
}
