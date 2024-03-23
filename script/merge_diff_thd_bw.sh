#!/bin/bash

# compete_local_rand_bandwidth_output.dat

cur_date=$1
bandwidth_data=$2
numa_node=$3
tmp_test_result=$4
csv_folder="/home/gxr/cxl_mem/microbench/data/merged_data/$cur_date"

mkdir -p "$csv_folder"


# 准备输出文件


output_file="$csv_folder/$bandwidth_data"
# output_file="$csv_folder compete_remote_rand_bandwidth_output.dat"
# output_file="$csv_folder compete_remote_rand_bandwidth_output.dat"
# output_file="$csv_folder compete_remote_rand_bandwidth_output.dat"

# 写入 Gnuplot 绘图的头部信息
echo -e "Threads\tclwb sfence\tclwb\tns sfence\tns store\tclf_load\tload" > "$output_file"

# 处理每个文件
for file in /home/gxr/cxl_mem/microbench/data/$tmp_test_result/$cur_date/Calc.*.thd.numa$numa_node.log_rand.csv; do
    threads=$(echo "$file" | grep -oP '\d+(?=\.thd)') # 提取文件名中的数字作为线程数
    echo -n "$threads" >> "$output_file" # 写入线程数到输出文件

    # 提取延迟数据并写入输出文件
    awk -F, 'NR>1 {printf "\t%s",$6}' "$file" >> "$output_file"

    echo "" >> "$output_file" # 换行
done
