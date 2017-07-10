#!/usr/bin/python3

import babeltrace.reader as btr
import babeltrace.writer as btw
import sys
from hsa_events import event_classes

if len(sys.argv) < 4:
    print('Syntax: ./hsaqp_event_integration.py INPUT_TRACE_1 INPUT_TRACE_2 OUTPUT_TRACE')
    sys.exit()

# Add the input traces to the collection
trace_path = '/ust/uid/1000/64-bit/'
in_path_1, in_path_2 = sys.argv[1] + trace_path, sys.argv[2] + trace_path
collection = btr.TraceCollection()
collection.add_trace(in_path_1, 'ctf')
collection.add_trace(in_path_2, 'ctf')
print('Read input traces at {} and {}.'.format(sys.argv[1], sys.argv[2]))

# Set the output trace
out_path = sys.argv[3]
print('Output trace will be located at {}.'.format(out_path))
writer = btw.Writer(out_path)

# Create the clock and register it to the writer
main_clock = btw.Clock('main_clock')
writer.add_clock(main_clock)

# Create stream class
main_stream_class = btw.StreamClass('main_stream')
main_stream_class.clock = main_clock

# Create stream
for event_class in event_classes.values():
    main_stream_class.add_event_class(event_class)
main_stream = writer.create_stream(main_stream_class)

# Create events, based on event classes

for r_event in collection.events:
    name = r_event.name.split(':')[-1]
    w_event = btw.Event(event_classes[name])

    if name == 'function_entry' or name == 'function_exit':
        w_event.payload('name').value = r_event['name']

    elif name == 'queue_created':
        w_event.payload('agent_handle').value = r_event['agent_handle']
        w_event.payload('queue_id').value = r_event['queue_id']

    elif name == 'queue_destroyed':
        w_event.payload('queue_id').value = r_event['queue_id']

    elif name == 'aql_kernel_dispatch_packet_submitted':
        w_event.payload('packet_id').value = r_event['packet_id']
        w_event.payload('agent_handle').value = r_event['agent_handle']
        w_event.payload('queue_id').value = r_event['queue_id']
        w_event.payload('kernel_object').value = r_event['kernel_object']
        w_event.payload('kernel_name').value = r_event['kernel_name']

    elif name == 'kernel_start_nm' or name == 'kernel_end_nm':
        w_event.payload('kernel_object').value = r_event['kernel_object']
        w_event.payload('kernel_name').value = r_event['kernel_name']
        w_event.payload('agent_handle').value = r_event['agent_handle']
        w_event.payload('queue_id').value = r_event['queue_id']
        w_event.payload('timestamp').value = r_event['timestamp']

    else:
        w_event.payload('name').value = name

    main_clock.time = r_event.timestamp
    main_stream.append_event(w_event)

# Flush the stream
main_stream.flush()
