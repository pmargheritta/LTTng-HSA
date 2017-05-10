#ifndef UTIL_H
#define UTIL_H

#include <dlfcn.h>
#include <iostream>
#include <map>
#include <hsa.h>
#include "hsa_tracepoints.h"

extern uint64_t init_timestamp;
extern std::map<uint64_t, const char*> symbol_names;
extern std::map<uint64_t, uint64_t> kernel_object_symbols;

uint64_t mc_timestamp();
hsa_status_t my_hsa_init();
hsa_status_t my_hsa_executable_get_symbol(
    hsa_executable_t executable,
    const char *module_name,
    const char *symbol_name,
    hsa_agent_t agent,
    int32_t call_convention,
    hsa_executable_symbol_t *symbol);
hsa_status_t my_hsa_executable_symbol_get_info(
    hsa_executable_symbol_t executable_symbol,
    hsa_executable_symbol_info_t attribute,
    void *value);

#endif
