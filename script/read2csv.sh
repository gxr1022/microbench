#!/bin/bash

cur_date=$1
tmp_test_result=$2
tmp_data=$3
# logs_folder="/home/gxr/cxl_mem/microbench/src/tmp_single_test/2023-11-30-18-11-21"
logs_folder="/home/gxr/cxl_mem/microbench/src/$tmp_test_result/$cur_date"
# logs_folder="/home/gxr/cxl_mem/microbench/src/tmp_compM_test/2024-02-20-03-09-41"
# csv_folder="/home/gxr/cxl_mem/microbench/data/thread_compete_mem/$cur_date"
csv_folder="/home/gxr/cxl_mem/microbench/data/$tmp_data/$cur_date"
# csv_folder="/home/gxr/cxl_mem/microbench/data/single_test/$cur_date"


mkdir -p "$csv_folder"

for logfile in "$logs_folder"/*.log; do
    filename=$(basename "$logfile")
    echo "$logfile"

    seq_csv_file="$csv_folder/${filename}_seq.csv"
    rand_csv_file="$csv_folder/${filename}_rand.csv"

    # 创建CSV文件并写入标题行
    echo "类型,Trav,Order,延迟(ns),带宽(ops),带宽(MB/s)" > "$seq_csv_file"
    echo "类型,Trav,Order,延迟(ns),带宽(ops),带宽(MB/s)" > "$rand_csv_file"

    # 提取 type、Trav、Order、延迟和带宽信息到CSV文件
    grep -A5 -E '\[type\]:|\[Latency\]:|\[Bandwidth_ops\]:|\[Bandwidth_mbs\]:' "$logfile" | awk -F'[][]' -v seq_csv="$seq_csv_file" -v rand_csv="$rand_csv_file" '
    BEGIN {
        OFS=","
        type = ""
        trav = ""
        order = ""
        latency = ""
        bandwidth_ops = ""
        bandwidth_mbs = ""
    }
    /\[type\]:/ {
        type = $4
    }
    /\[Trav\]:/ {
        trav = $8
    }
    /\[order\]:\[seq\]/ {
        order = "seq"
    }
    /\[order\]:\[rand\]/ {
        order = "rand"
    }
    /\[Latency\]:/ {
        latency = $4
    }
    /\[Bandwidth_ops\]:/ {
        bandwidth_ops = $4
    }
    /\[Bandwidth_mbs\]:/ {
        bandwidth_mbs = $4
        gsub(/[^0-9.]/, "", latency)
        gsub(/[^0-9.]/, "", bandwidth_ops)
        gsub(/[^0-9.]/, "", bandwidth_mbs)
        if (order == "seq") {
            print type, trav, order, latency, bandwidth_ops, bandwidth_mbs >> seq_csv
        } else if (order == "rand") {
            print type, trav, order, latency, bandwidth_ops, bandwidth_mbs >> rand_csv
        }
    }
    '

    echo "提取完成。数据已保存到 $seq_csv_file 和 $rand_csv_file 文件中。"

done
