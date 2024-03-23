#!/bin/bash

# 定义日志文件路径和CSV文件路径
cur_date=$1
tmp_test_result=$2

logs_folder="/home/gxr/cxl_mem/microbench/src/$tmp_test_result/$cur_date"
csv_folder="/home/gxr/cxl_mem/microbench/data/single_test/$tmp_test_result/$cur_date"

mkdir -p "$csv_folder"


for logfile in "$logs_folder"/*.log; do
    filename=$(basename "$logfile")
    echo $logfile
    csv_file="$csv_folder/${filename}.csv"


    # 创建CSV文件并写入标题行
    echo "类型,Trav,Order,延迟(ns),带宽(ops),带宽(MB/s)" > "$csv_file"

    # 提取 type、Trav、Order、延迟和带宽信息到CSV文件
    grep -E '\[type\]|\[Trav\]|\[order\]|\[Latency\]|\[Bandwidth_ops\]|\[Bandwidth_mbs\]' "$logfile" | awk -F'[][]' '
    BEGIN {
        OFS=","
        type = ""
        trav = ""
        order = ""
        latency = ""
        bandwidth_ops = ""
        bandwidth_mbs = ""
    }
    /\[type\]/ {
        type = $4
    }
    /\[Trav\]/ {
        trav = $8
    }
    /\[order\]/ {
        order = $12
    }
    /\[Latency\]/ {
        latency = $4
    }
    /\[Bandwidth_ops\]/ {
        bandwidth_ops = $4
    }
    /\[Bandwidth_mbs\]/ {
        bandwidth_mbs = $4
        gsub(/[^0-9.]/, "", latency)
        gsub(/[^0-9.]/, "", bandwidth_ops)
        gsub(/[^0-9.]/, "", bandwidth_mbs)
        print type, trav, order, latency, bandwidth_ops, bandwidth_mbs >> "'$csv_file'"
    }
    '
    echo "提取完成。数据已保存到 $csv_file 文件中。"

done