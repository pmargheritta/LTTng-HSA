#!/usr/bin/python3

import babeltrace.reader as btr
import babeltrace.writer as btw
import sys
from hsa_events import event_classes

if len(sys.argv) < 3:
    print('Syntax: ./hsaqp_event_integration.py INPUT_TRACE OUTPUT_TRACE')
    sys.exit()

# Add the input trace to the collection
collection = btr.TraceCollection()
collection.add_trace(sys.argv[1], 'ctf')
print('Read input trace at {}.'.format(sys.argv[1]))

# Set the output trace
out_path = sys.argv[2]
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

init_time = None
events = {}

for r_event in collection.events:
    name = r_event.name.split(':')[-1]
    event_time = r_event.timestamp
    w_event = btw.Event(event_classes[name])

    if name == 'function_entry':
        if not init_time and r_event['name'] == 'hsa_init':
            init_time = event_time
        w_event.payload('name').value = r_event['name']

    elif name == 'function_exit':
        w_event.payload('name').value = r_event['name']

    elif name == 'queue_created':
        w_event.payload('agent_handle').value = r_event['agent_handle']
        w_event.payload('queue_id').value = r_event['queue_id']

    elif name == 'queue_destroyed':
        queue_destroyed_event.payload('queue_id').value = event['queue_id']

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
        event_time = init_time + r_event['timestamp']

    else:
        w_event.payload('name').value = name

    events[event_time] = w_event

# Append events to the stream
timestamps = list(events.keys())
timestamps.sort()
for timestamp in timestamps:
    main_clock.time = timestamp
    main_stream.append_event(events[timestamp])

# Flush the stream
main_stream.flush()
