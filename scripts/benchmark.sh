#!/bin/bash

repeats=$1
output='benchmark.csv'

echo "#,no HSA-TT,call_stack,kernel_times,queue_profiling,perf_counters" >> $output

for (( i = 1; i <= $repeats; i++ ))
do
    time_no=$( TIMEFORMAT=%E; time ( ./vector_copy > /dev/null ) 2>&1 )
    time_cs=$( TIMEFORMAT=%E; time ( ../scripts/hsa_call_stack.sh > /dev/null ) 2>&1 )
    time_kt=$( TIMEFORMAT=%E; time ( ../scripts/hsa_kernel_times.sh > /dev/null ) 2>&1 )
    time_qp=$( TIMEFORMAT=%E; time ( ../scripts/hsa_queue_profiling.sh > /dev/null ) 2>&1 )
    time_pc=$( TIMEFORMAT=%E; time ( ../scripts/hsa_perf_counters.sh > /dev/null ) 2>&1 )
    echo $i','$time_no','$time_cs','$time_kt','$time_qp','$time_pc >> $output
    
    p=$(( $i * 100 / $repeats ))
    echo -ne $p'%\r'
done
