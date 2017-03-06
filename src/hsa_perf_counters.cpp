#include <dlfcn.h>
#include <unistd.h>
#include <iostream>

#include <hsa.h>
#include <amd_hsa_tools_interfaces.h>
#include <GPUPerfAPI-HSA.h>

// TODO créer header
// TODO uniformiser conventions nommage

typedef struct pre_dispatch_callback_args_s {
    gpa_uint32 *session_id;
    hsa_ext_tools_pre_dispatch_callback_function orig_callback;
    void *orig_args;
} pre_dispatch_callback_args_t;

typedef struct post_dispatch_callback_args_s {
    gpa_uint32 *session_id;
    hsa_ext_tools_post_dispatch_callback_function orig_callback;
    void *orig_args;
} post_dispatch_callback_args_t;

void pre_dispatch_callback(const hsa_dispatch_callback_t* dispatch_params, void* user_args)
{
    pre_dispatch_callback_args_t* args = (pre_dispatch_callback_args_t*) user_args;

    gpa_uint32 numCounters, passCount;
    GPA_GetNumCounters(&numCounters);

    // Enable counters that require only one pass
    for (int counter = 0; counter < numCounters; counter++) {
        GPA_EnableCounter(counter);
        GPA_GetPassCount(&passCount);
        if (passCount > 1)
            GPA_DisableCounter(counter);
    }

    GPA_BeginSession(args->session_id);

    // Sample the enabled counters
    GPA_BeginPass();
    GPA_BeginSample(0);
    args->orig_callback(dispatch_params, args->orig_args);
}

void read_gpa_counters(gpa_uint32 sessionId) {
    // Fetch the values of the counters
    bool sessionReady = false;

    if (sessionReady) {
        std::cout << "Session ready" << std::endl;

        gpa_uint32 sampleCount;
        GPA_GetSampleCount(sessionId, &sampleCount);
        std::cout << "Sample count: " << sampleCount << std::endl;

        // For each sample in the session
        for (int sample = 0; sample < sampleCount; sample++) {
            bool sampleReady;
            GPA_IsSampleReady(&sampleReady, sessionId, sample);

            if (sampleReady) {
                std::cout << "Sample " << sample << " ready" << std::endl;

                gpa_uint32 enabledCount, enabledCounterIndex;
                const char* counterName;
                GPA_Type type;
                GPA_GetEnabledCount(&enabledCount);

                // For each counter in the sample
                for (int counter = 0; counter < enabledCount; counter++) {
                    GPA_GetEnabledIndex(counter, &enabledCounterIndex);
                    GPA_GetCounterName(enabledCounterIndex, &counterName);
                    GPA_GetCounterDataType(enabledCounterIndex, &type);

                    if (type == GPA_TYPE_UINT32) {
                        gpa_uint32 value;
                        GPA_GetSampleUInt32(sessionId, sample, enabledCounterIndex, &value);
                        std::cout << "#" << enabledCounterIndex << " " << counterName << "=" << value << std::endl;
                    }
                    else if (type == GPA_TYPE_UINT64) {
                        gpa_uint64 value;
                        GPA_GetSampleUInt64(sessionId, sample, enabledCounterIndex, &value);
                        std::cout << "#" << enabledCounterIndex << " " << counterName << "=" << value << std::endl;
                    }
                    else if (type == GPA_TYPE_FLOAT32) {
                        gpa_float32 value;
                        GPA_GetSampleFloat32(sessionId, sample, enabledCounterIndex, &value);
                        std::cout << "#" << enabledCounterIndex << " " << counterName << "=" << value << std::endl;
                    }
                    else if (type == GPA_TYPE_FLOAT64) {
                        gpa_float64 value;
                        GPA_GetSampleFloat64(sessionId, sample, enabledCounterIndex, &value);
                        std::cout << "#" << enabledCounterIndex << " " << counterName << "=" << value << std::endl;
                    }
                    else
                        assert(false);
                }
            }
        }
    }
}

void post_dispatch_callback(const hsa_dispatch_callback_t* dispatch_params, void* user_args)
{
    post_dispatch_callback_args_t* args = (post_dispatch_callback_args_t*) user_args;
    args->orig_callback(dispatch_params, args->orig_args);

    GPA_EndSample();
    GPA_EndPass();
    GPA_EndSession();

    read_gpa_counters(*(args->session_id));
}

void gpa_logging_callback(GPA_Logging_Type logging_type, const char* message) {
    std::cout << "[GPA log] " << message << std::endl;
}

hsa_status_t hsa_init()
{
    decltype(hsa_init) *orig = (decltype(hsa_init)*) dlsym(RTLD_NEXT, "hsa_init");
    GPA_Initialize();
    GPA_RegisterLoggingCallback(GPA_LOGGING_ALL, gpa_logging_callback);
    hsa_status_t retval = orig();
    return retval;
}

hsa_status_t hsa_shut_down()
{
    decltype(hsa_shut_down) *orig = (decltype(hsa_shut_down)*) dlsym(RTLD_NEXT, "hsa_shut_down");
    hsa_status_t retval = orig();
    GPA_Destroy();
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

    GPA_Status status = GPA_OpenContext(*queue);

    gpa_uint32 *sessionId = (gpa_uint32*) malloc(sizeof(gpa_uint32));

    hsa_ext_tools_pre_dispatch_callback_function orig_pre_dispatch_callback;
    hsa_ext_tools_post_dispatch_callback_function orig_post_dispatch_callback;
    hsa_ext_tools_get_callback_functions(*queue, orig_pre_dispatch_callback, orig_post_dispatch_callback);

    void *orig_pre_dispatch_callback_args;
    void *orig_post_dispatch_callback_args;
    hsa_ext_tools_get_callback_arguments(*queue, orig_pre_dispatch_callback_args, orig_post_dispatch_callback_args);

    pre_dispatch_callback_args_t *pre_dispatch_callback_args = (pre_dispatch_callback_args_t*) malloc(sizeof(pre_dispatch_callback_args_t));
    pre_dispatch_callback_args->session_id = sessionId;
    pre_dispatch_callback_args->orig_callback = orig_pre_dispatch_callback;
    pre_dispatch_callback_args->orig_args = orig_pre_dispatch_callback_args;

    post_dispatch_callback_args_t *post_dispatch_callback_args = (post_dispatch_callback_args_t*) malloc(sizeof(post_dispatch_callback_args_t));
    post_dispatch_callback_args->session_id = sessionId;
    post_dispatch_callback_args->orig_callback = orig_post_dispatch_callback;
    post_dispatch_callback_args->orig_args = orig_post_dispatch_callback_args;

    hsa_ext_tools_set_callback_functions(*queue, pre_dispatch_callback, post_dispatch_callback);
    hsa_ext_tools_set_callback_arguments(*queue, pre_dispatch_callback_args, post_dispatch_callback_args);

    return retval;
}

hsa_status_t hsa_queue_destroy(hsa_queue_t *queue)
{
    decltype(hsa_queue_destroy) *orig = (decltype(hsa_queue_destroy)*) dlsym(RTLD_NEXT, "hsa_queue_destroy");

    GPA_CloseContext();

    hsa_status_t retval = orig(queue);
    return retval;
}
