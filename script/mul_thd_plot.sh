CUR_DATE=("2024-02-20-09-33-32" "2024-02-20-11-01-27")
# cur_date=$1
bandwidth_data="compete_local_rand_bandwidth_output.dat"
tmp_test_result="tmp_compM_test"
tmp_data="thread_compete_mem"

TEST_TYPE=("thread_compete_mem" "thread_bind_mem")
TMP_DATA_TYPE=("thread_compete_mem" "thread_bind_mem")
BANDWIDTH_DATA=("compete_local_rand_bandwidth_output.dat" "compete_remote_rand_bandwidth_output.dat" "compete_remote_cxl_rand_bandwidth_output.dat" "compete_local_cxl_rand_bandwidth_output.dat")
NUMA_NODE=(0 1 2 3)

len=${#BANDWIDTH_DATA[@]}
l=${#TEST_TYPE[@]}
for ((j=0; j<l; j++)); do
  tmp_test_result=${TEST_TYPE[$j]}
  cur_date=${CUR_DATE[$j]}
  ./read2csv.sh $cur_date $tmp_test_result $tmp_test_result

  for ((i=0; i<len; i++)); do
    bandwidth_data=${BANDWIDTH_DATA[$i]}
    numa_node=${NUMA_NODE[$i]}

    ./merge_diff_thd_bw.sh  $cur_date $bandwidth_data $numa_node $tmp_test_result
    ./sort_data.sh $cur_date $bandwidth_data
    sorted_csv_folder="/home/gxr/cxl_mem/microbench/data/sorted_data/$cur_date"
    sorted_dat_file="$sorted_csv_folder/$bandwidth_data"
    output_dir="/home/gxr/cxl_mem/microbench/data/plot_graph/$tmp_test_result/bandwidth/$cur_date"
    mkdir -p $output_dir
    # output_png="$bandwidth_data.png"
    gnuplot -e "datafile='$sorted_dat_file'" -e "output_dir='$output_dir'" -e "output_png='$bandwidth_data.png'"  mul_thd_bw_plot.plt
  
  done

done




