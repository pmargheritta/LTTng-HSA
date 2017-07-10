#!/usr/bin/python3

import babeltrace.reader as btr
import babeltrace.writer as btw
from collections import defaultdict

# Create field declarations

uint64_fd = btw.IntegerFieldDeclaration(64)
uint64_fd.signed = False

huint64_fd = btw.IntegerFieldDeclaration(64)
huint64_fd.base = btw.IntegerBase.HEX
huint64_fd.signed = False

string_fd = btw.StringFieldDeclaration()

# Create event classes

def event_classes_factory():
    other_event_class = btw.EventClass('hsa_runtime:other_event')
    other_event_class.add_field(string_fd, 'name')
    return other_event_class

event_classes = defaultdict(event_classes_factory)

event_classes['runtime_initialized'] = btw.EventClass('hsa_runtime:runtime_initialized')
event_classes['runtime_shut_down'] = btw.EventClass('hsa_runtime:runtime_shut_down')

event_classes['function_entry'] = btw.EventClass('hsa_runtime:function_entry')
event_classes['function_entry'].add_field(string_fd, 'name')

event_classes['function_exit'] = btw.EventClass('hsa_runtime:function_exit')
event_classes['function_exit'].add_field(string_fd, 'name')

event_classes['queue_created'] = btw.EventClass('hsa_runtime:queue_created')
event_classes['queue_created'].add_field(huint64_fd, 'agent_handle')
event_classes['queue_created'].add_field(uint64_fd, 'queue_id')

event_classes['queue_destroyed'] = btw.EventClass('hsa_runtime:queue_destroyed')
event_classes['queue_destroyed'].add_field(uint64_fd, 'queue_id')

event_classes['aql_kernel_dispatch_packet_submitted'] = btw.EventClass('hsa_runtime:aql_kernel_dispatch_packet_submitted')
event_classes['aql_kernel_dispatch_packet_submitted'].add_field(uint64_fd, 'packet_id')
event_classes['aql_kernel_dispatch_packet_submitted'].add_field(huint64_fd, 'agent_handle')
event_classes['aql_kernel_dispatch_packet_submitted'].add_field(uint64_fd, 'queue_id')
event_classes['aql_kernel_dispatch_packet_submitted'].add_field(huint64_fd, 'kernel_object')
event_classes['aql_kernel_dispatch_packet_submitted'].add_field(string_fd, 'kernel_name')

event_classes['kernel_start_nm'] = btw.EventClass('hsa_runtime:kernel_start_nm')
event_classes['kernel_start_nm'].add_field(huint64_fd, 'kernel_object')
event_classes['kernel_start_nm'].add_field(string_fd, 'kernel_name')
event_classes['kernel_start_nm'].add_field(huint64_fd, 'agent_handle')
event_classes['kernel_start_nm'].add_field(uint64_fd, 'queue_id')
event_classes['kernel_start_nm'].add_field(uint64_fd, 'timestamp')

event_classes['kernel_end_nm'] = btw.EventClass('hsa_runtime:kernel_end_nm')
event_classes['kernel_end_nm'].add_field(huint64_fd, 'kernel_object')
event_classes['kernel_end_nm'].add_field(string_fd, 'kernel_name')
event_classes['kernel_end_nm'].add_field(huint64_fd, 'agent_handle')
event_classes['kernel_end_nm'].add_field(uint64_fd, 'queue_id')
event_classes['kernel_end_nm'].add_field(uint64_fd, 'timestamp')

event_classes['kernel_start'] = btw.EventClass('hsa_runtime:kernel_start')
event_classes['kernel_start'].add_field(huint64_fd, 'kernel_object')
event_classes['kernel_start'].add_field(string_fd, 'kernel_name')
event_classes['kernel_start'].add_field(huint64_fd, 'agent_handle')
event_classes['kernel_start'].add_field(uint64_fd, 'queue_id')

event_classes['kernel_end'] = btw.EventClass('hsa_runtime:kernel_end')
event_classes['kernel_end'].add_field(huint64_fd, 'kernel_object')
event_classes['kernel_end'].add_field(string_fd, 'kernel_name')
event_classes['kernel_end'].add_field(huint64_fd, 'agent_handle')
event_classes['kernel_end'].add_field(uint64_fd, 'queue_id')
