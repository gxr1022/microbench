#!/bin/bash

set -x

log_dir="../src/tmp_single_test"

THREAD_NUM=(1)  
EXEC_PATH="../src/build/LatencyTest.exec"
TRAVERSE_TYPE=("false")
# TRAVERSE_TYPE=("false" "true")
NUMA_NODE=( 0 1 )
cur_date=`date "+%Y-%m-%d-%H-%M-%S"`
bench_log_path="$log_dir/$cur_date"
mkdir -p $bench_log_path

function doOneBenchmark() {
    bench_type=$1
    thread_num=$2
    numa_node=$3

    if [ $bench_type == "false" ]; then
        tra_type="Calc"
    else
        tra_type="Ptr"
    fi

    one_bench_log_prefix="$tra_type.$thread_num.thd.numa$numa_node.64"

    log_name="$one_bench_log_prefix.log"
    log_path="$bench_log_path/$log_name"

    bench_cmd="$EXEC_PATH --thread_number=$thread_num --traverse_type=$bench_type --numa_node=$numa_node  | tee $log_path"
    echo $bench_cmd
    eval $bench_cmd
}

for traverse_type in "${TRAVERSE_TYPE[@]}"; do
  echo $traverse_type 
  
  for numa_id in "${NUMA_NODE[@]}"; do
    
    for thd_n in "${THREAD_NUM[@]}"; do 
      echo $bench
      doOneBenchmark  "$traverse_type" $thd_n $numa_id
    done

  done

done
 

