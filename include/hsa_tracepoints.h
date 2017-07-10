#undef TRACEPOINT_PROVIDER
#define TRACEPOINT_PROVIDER hsa_runtime

#undef TRACEPOINT_INCLUDE
#define TRACEPOINT_INCLUDE "./hsa_tracepoints.h"

#if !defined(HSA_TRACEPOINTS_H) || defined(TRACEPOINT_HEADER_MULTI_READ)
#define HSA_TRACEPOINTS_H

#include <lttng/tracepoint.h>
#include <GPUPerfAPI-HSA.h>

TRACEPOINT_EVENT(
	hsa_runtime,
	runtime_initialized,
	TP_ARGS(),
	TP_FIELDS()
)

TRACEPOINT_EVENT(
	hsa_runtime,
	runtime_shut_down,
	TP_ARGS(),
	TP_FIELDS()
)

/* call_stack */

TRACEPOINT_EVENT(
	hsa_runtime,
	function_entry,
	TP_ARGS(
		char*, name
	),
	TP_FIELDS(
		ctf_string(name, name)
	)
)

TRACEPOINT_EVENT(
	hsa_runtime,
	function_exit,
	TP_ARGS(
		char*, name
	),
	TP_FIELDS(
		ctf_string(name, name)
	)
)

/* queue_profiling */

TRACEPOINT_EVENT(
	hsa_runtime,
	queue_created,
	TP_ARGS(
		uint64_t, agent_handle,
		uint64_t, queue_id
	),
	TP_FIELDS(
		ctf_integer_hex(uint64_t, agent_handle, agent_handle)
		ctf_integer(uint64_t, queue_id, queue_id)
	)
)

TRACEPOINT_EVENT(
	hsa_runtime,
	queue_destroyed,
	TP_ARGS(
		uint64_t, queue_id
	),
	TP_FIELDS(
		ctf_integer(uint64_t, queue_id, queue_id)
	)
)

TRACEPOINT_EVENT(
	hsa_runtime,
	aql_packet_submitted,
	TP_ARGS(
		uint64_t, packet_id,
		const char*, packet_type,
		uint64_t, agent_handle,
		uint64_t, queue_id
	),
	TP_FIELDS(
		ctf_integer(uint64_t, packet_id, packet_id)
		ctf_string(packet_type, packet_type)
		ctf_integer_hex(uint64_t, agent_handle, agent_handle)
		ctf_integer(uint64_t, queue_id, queue_id)
	)
)

TRACEPOINT_EVENT(
	hsa_runtime,
	aql_kernel_dispatch_packet_submitted,
	TP_ARGS(
		uint64_t, packet_id,
		uint64_t, agent_handle,
		uint64_t, queue_id,
		uint64_t, kernel_object,
		const char*, kernel_name
	),
	TP_FIELDS(
		ctf_integer(uint64_t, packet_id, packet_id)
		ctf_integer_hex(uint64_t, agent_handle, agent_handle)
		ctf_integer(uint64_t, queue_id, queue_id)
		ctf_integer_hex(uint64_t, kernel_object, kernel_object)
		ctf_string(kernel_name, kernel_name)
	)
)

/* kernel_times */

TRACEPOINT_EVENT(
	hsa_runtime,
	kernel_start_nm,
	TP_ARGS(
		uint64_t, kernel_object,
		const char*, kernel_name,
		uint64_t, agent_handle,
		uint64_t, queue_id,
		uint64_t, timestamp
	),
	TP_FIELDS(
		ctf_integer_hex(uint64_t, kernel_object, kernel_object)
		ctf_string(kernel_name, kernel_name)
		ctf_integer_hex(uint64_t, agent_handle, agent_handle)
		ctf_integer(uint64_t, queue_id, queue_id)
		ctf_integer(uint64_t, timestamp, timestamp)
	)
)

TRACEPOINT_EVENT(
	hsa_runtime,
	kernel_end_nm,
	TP_ARGS(
		uint64_t, kernel_object,
		const char*, kernel_name,
		uint64_t, agent_handle,
		uint64_t, queue_id,
		uint64_t, timestamp
	),
	TP_FIELDS(
		ctf_integer_hex(uint64_t, kernel_object, kernel_object)
		ctf_string(kernel_name, kernel_name)
		ctf_integer_hex(uint64_t, agent_handle, agent_handle)
		ctf_integer(uint64_t, queue_id, queue_id)
		ctf_integer(uint64_t, timestamp, timestamp)
	)
)

/* perf_counters */

TRACEPOINT_EVENT(
	hsa_runtime,
	kernel_perf_counter_uint32_nm,
	TP_ARGS(
		uint64_t, kernel_object,
		gpa_uint32, counter_index,
		const char*, counter_name,
		uint32_t, value
	),
	TP_FIELDS(
		ctf_integer_hex(uint64_t, kernel_object, kernel_object)
		ctf_integer(uint32_t, counter_index, counter_index)
		ctf_string(counter_name, counter_name)
		ctf_integer(uint32_t, value, value)
	)
)

TRACEPOINT_EVENT(
	hsa_runtime,
	kernel_perf_counter_uint64_nm,
	TP_ARGS(
		uint64_t, kernel_object,
		gpa_uint32, counter_index,
		const char*, counter_name,
		uint64_t, value
	),
	TP_FIELDS(
		ctf_integer_hex(uint64_t, kernel_object, kernel_object)
		ctf_integer(uint32_t, counter_index, counter_index)
		ctf_string(counter_name, counter_name)
		ctf_integer(uint64_t, value, value)
	)
)

TRACEPOINT_EVENT(
	hsa_runtime,
	kernel_perf_counter_float32_nm,
	TP_ARGS(
		uint64_t, kernel_object,
		gpa_uint32, counter_index,
		const char*, counter_name,
		float, value
	),
	TP_FIELDS(
		ctf_integer_hex(uint64_t, kernel_object, kernel_object)
		ctf_integer(uint32_t, counter_index, counter_index)
		ctf_string(counter_name, counter_name)
		ctf_float(float, value, value)
	)
)

TRACEPOINT_EVENT(
	hsa_runtime,
	kernel_perf_counter_float64_nm,
	TP_ARGS(
		uint64_t, kernel_object,
		gpa_uint32, counter_index,
		const char*, counter_name,
		double, value
	),
	TP_FIELDS(
		ctf_integer_hex(uint64_t, kernel_object, kernel_object)
		ctf_integer(uint32_t, counter_index, counter_index)
		ctf_string(counter_name, counter_name)
		ctf_float(double, value, value)
	)
)

#endif /* HSA_TRACEPOINTS_H */

#include <lttng/tracepoint-event.h>
