#include "util.h"

uint64_t init_timestamp;
std::map<uint64_t, const char*> symbol_names;
std::map<uint64_t, uint64_t> kernel_object_symbols;

uint64_t mc_timestamp()
{
    uint64_t timestamp;
    hsa_system_get_info(HSA_SYSTEM_INFO_TIMESTAMP, &timestamp);
    return timestamp;
}

hsa_status_t my_hsa_init()
{
    decltype(hsa_init) *orig = (decltype(hsa_init)*) dlsym(RTLD_NEXT, "hsa_init");
    hsa_status_t retval = orig();
    init_timestamp = mc_timestamp();
    return retval;
}

hsa_status_t my_hsa_executable_get_symbol(
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

hsa_status_t my_hsa_executable_symbol_get_info(
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
