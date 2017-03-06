#undef TRACEPOINT_PROVIDER
#define TRACEPOINT_PROVIDER hsa_runtime

#undef TRACEPOINT_INCLUDE
#define TRACEPOINT_INCLUDE "./hsa_tracepoints.h"

#if !defined(HSA_TRACEPOINTS_H) || defined(TRACEPOINT_HEADER_MULTI_READ)
#define HSA_TRACEPOINTS_H

#include <lttng/tracepoint.h>

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

TRACEPOINT_EVENT(
	hsa_runtime,
	runtime_initialized,
	TP_ARGS(),
	TP_FIELDS()
)

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
	aql_kernel_dispatch_packet_submitted,
	TP_ARGS(
		uint64_t, packet_id,
		uint64_t, agent_handle,
		uint64_t, queue_id,
		uint64_t, kernel_id
	),
	TP_FIELDS(
		ctf_integer(uint64_t, packet_id, packet_id)
		ctf_integer_hex(uint64_t, agent_handle, agent_handle)
		ctf_integer(uint64_t, queue_id, queue_id)
		ctf_integer_hex(uint64_t, kernel_id, kernel_id)
	)
)

TRACEPOINT_EVENT(
	hsa_runtime,
	kernel_times_gathered,
	TP_ARGS(
		uint64_t, kernel_id,
		uint64_t, agent_handle,
		uint64_t, queue_id,
		uint64_t, kernel_start_time,
		uint64_t, kernel_end_time
	),
	TP_FIELDS(
		ctf_integer_hex(uint64_t, kernel_id, kernel_id)
		ctf_integer_hex(uint64_t, agent_handle, agent_handle)
		ctf_integer(uint64_t, queue_id, queue_id)
		ctf_integer(uint64_t, kernel_start_time, kernel_start_time)
		ctf_integer(uint64_t, kernel_end_time, kernel_end_time)
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
	runtime_shut_down,
	TP_ARGS(),
	TP_FIELDS()
)

#endif /* HSA_TRACEPOINTS_H */

#include <lttng/tracepoint-event.h>
