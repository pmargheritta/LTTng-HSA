#include "hsa_perf_counters.h"

void pre_dispatch_callback(const hsa_dispatch_callback_t* dispatch_params, void* user_args)
{
    pthread_mutex_lock(&mutex);

    if (session_id > 0)
        close_context(session_id);

    GPA_HSA_Context *context;
    context->m_pAgent = &(dispatch_params->agent);
    context->m_pQueue = dispatch_params->queue;
    context->m_pAqlTranslationHandle = dispatch_params->aql_translation_handle;
    GPA_OpenContext(context);

    gpa_uint32 numCounters, passCount;
    GPA_GetNumCounters(&numCounters);

    // Enable counters that require only one pass
    // TODO allow more passes
    for (int counter = 0; counter < numCounters; counter++) {
        GPA_EnableCounter(counter);
        GPA_GetPassCount(&passCount);
        if (passCount > 1)
            GPA_DisableCounter(counter);
    }

    GPA_BeginSession(&session_id);
    session_kernel_objects[session_id] = dispatch_params->aql_packet->kernel_object;

    // Sample the enabled counters
    GPA_BeginPass();
    GPA_BeginSample(0);
}

void post_dispatch_callback(const hsa_dispatch_callback_t* dispatch_params, void* user_args)
{
    GPA_EndSample();
    GPA_EndPass();
    GPA_EndSession();

    pthread_mutex_unlock(&mutex);
}

void close_context(gpa_uint32 session_id)
{
    // Close the context opened for the previous kernel
    // These operations do not work when performed in the post-dispatch callback
    read_gpa_counters(session_id);
    GPA_CloseContext();
}

void read_gpa_counters(gpa_uint32 session_id)
{
    bool session_ready;
    GPA_IsSessionReady(&session_ready, session_id);

    if (session_ready) {
        gpa_uint32 session_count;
        GPA_GetSampleCount(session_id, &session_count);

        // For each sample in the session
        for (int sample = 0; sample < session_count; sample++) {
            bool sample_ready;
            GPA_IsSampleReady(&sample_ready, session_id, sample);

            if (sample_ready) {
                gpa_uint32 enabled_count, enabled_counter_index;
                const char* counter_name;
                GPA_Type type;
                GPA_GetEnabledCount(&enabled_count);

                // For each counter in the sample
                for (int counter = 0; counter < enabled_count; counter++) {
                    GPA_GetEnabledIndex(counter, &enabled_counter_index);
                    GPA_GetCounterName(enabled_counter_index, &counter_name);
                    GPA_GetCounterDataType(enabled_counter_index, &type);

                    if (type == GPA_TYPE_UINT32) {
                        gpa_uint32 value;
                        GPA_GetSampleUInt32(session_id, sample, enabled_counter_index, &value);
                        tracepoint(hsa_runtime, kernel_perf_counter_uint32_nm, session_kernel_objects[session_id], enabled_counter_index, counter_name, value);
                    }
                    else if (type == GPA_TYPE_UINT64) {
                        gpa_uint64 value;
                        GPA_GetSampleUInt64(session_id, sample, enabled_counter_index, &value);
                        tracepoint(hsa_runtime, kernel_perf_counter_uint64_nm, session_kernel_objects[session_id], enabled_counter_index, counter_name, value);
                    }
                    else if (type == GPA_TYPE_FLOAT32) {
                        gpa_float32 value;
                        GPA_GetSampleFloat32(session_id, sample, enabled_counter_index, &value);
                        tracepoint(hsa_runtime, kernel_perf_counter_float32_nm, session_kernel_objects[session_id], enabled_counter_index, counter_name, value);
                    }
                    else if (type == GPA_TYPE_FLOAT64) {
                        gpa_float64 value;
                        GPA_GetSampleFloat64(session_id, sample, enabled_counter_index, &value);
                        tracepoint(hsa_runtime, kernel_perf_counter_float64_nm, session_kernel_objects[session_id], enabled_counter_index, counter_name, value);
                    }
                    else
                        assert(false);
                }
            }
        }
    }
}

void gpa_logging_callback(GPA_Logging_Type logging_type, const char* message)
{
    std::cout << "[GPA log] " << message << std::endl;
}

hsa_status_t hsa_init()
{
    decltype(hsa_init) *orig = (decltype(hsa_init)*) dlsym(RTLD_NEXT, "hsa_init");

    GPA_RegisterLoggingCallback(GPA_LOGGING_ALL, gpa_logging_callback);
    pthread_mutex_init(&mutex, nullptr);
    session_id = 0;

    hsa_status_t retval = orig();
    return retval;
}

hsa_status_t hsa_queue_create(hsa_agent_t agent, uint32_t size, hsa_queue_type_t type,
    void (*callback)(hsa_status_t status, hsa_queue_t *source,
    void *data),
    void *data, uint32_t private_segment_size,
    uint32_t group_segment_size, hsa_queue_t **queue)
{
    decltype(hsa_queue_create) *orig = (decltype(hsa_queue_create)*) dlsym(RTLD_NEXT, "hsa_queue_create");
    hsa_status_t retval = orig(agent, size, type, callback, data, private_segment_size, group_segment_size, queue);

    hsa_ext_tools_set_callback_functions(*queue, pre_dispatch_callback, post_dispatch_callback);

    return retval;
}

hsa_status_t hsa_shut_down()
{
    decltype(hsa_shut_down) *orig = (decltype(hsa_shut_down)*) dlsym(RTLD_NEXT, "hsa_shut_down");

    pthread_mutex_lock(&mutex);
    close_context(session_id);
    pthread_mutex_unlock(&mutex);

    pthread_mutex_destroy(&mutex);

    hsa_status_t retval = orig();
    return retval;
}
