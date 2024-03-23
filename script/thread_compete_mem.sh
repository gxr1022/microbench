#!/bin/bash

set -x

# cd ../src/build
# rm -rf ./*
# cmake -DDCOMPETE_MEM=ON ..
# make

# cd ../../script


log_dir="../src/thread_compete_mem"

pool_bits=30

THREAD_NUM=(1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24)  
# THREAD_NUM=(21 22 23 24)
EXEC_PATH="../src/build/LatencyTest.exec"
# TRAVERSE_TYPE=("false" "true")
TRAVERSE_TYPE=("false")
# NUMA_NODE=( 0 1 2)
NUMA_NODE=(2)
# cur_date=`date "+%Y-%m-%d-%H-%M-%S"`
cur_date="2024-02-20-09-33-32"
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

    one_bench_log_prefix="$tra_type.$thread_num.thd.numa$((numa_node + 1))"
    # one_bench_log_prefix="$tra_type.$thread_num.thd.numa$numa_node"

    log_name="$one_bench_log_prefix.log"
    log_path="$bench_log_path/$log_name"

    bench_cmd="$EXEC_PATH --thread_number=$thread_num --traverse_type=$bench_type --numa_node=$numa_node --pool_bits=$pool_bits  | tee $log_path"
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
 

