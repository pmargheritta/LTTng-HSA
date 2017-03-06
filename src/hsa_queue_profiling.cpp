#include <dlfcn.h>
#include <iostream>

#include <hsa.h>
#include <hsa_ext_profiler.h>

hsa_status_t hsa_queue_create(hsa_agent_t agent, uint32_t size, hsa_queue_type_t type,
    void (*callback)(hsa_status_t status, hsa_queue_t *source,
    void *data),
    void *data, uint32_t private_segment_size,
    uint32_t group_segment_size, hsa_queue_t **queue)
{
    decltype(hsa_queue_create) *orig = hsa_ext_tools_queue_create_profiled;
    hsa_status_t retval = orig(agent, size, type, callback, data, private_segment_size, group_segment_size, queue);
    return retval;
}

hsa_status_t hsa_queue_destroy(hsa_queue_t *queue)
{
    decltype(hsa_queue_destroy) *orig = (decltype(hsa_queue_destroy)*) dlsym(RTLD_NEXT, "hsa_queue_destroy");

    size_t count = hsa_ext_tools_get_kernel_times(0, NULL);
    if (count > 0) {
        hsa_profiler_kernel_time_t* records = new(std::nothrow) hsa_profiler_kernel_time_t[count];
        count = hsa_ext_tools_get_kernel_times(count, records);
        std::cout << "Count: " << count << std::endl;

        hsa_profiler_kernel_time_t record;
        for (int i = 0; i < count; i++) {
            record = records[i];
            std::cout << "kernel start: " << record.time.start << std::endl;
            std::cout << "kernel end:   " << record.time.end << std::endl;
        }
    }

    hsa_status_t retval = orig(queue);
    return retval;
}
