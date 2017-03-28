#include "util.h"

uint64_t mc_timestamp()
{
    uint64_t timestamp;
    hsa_system_get_info(HSA_SYSTEM_INFO_TIMESTAMP, &timestamp);
    return timestamp;
}
