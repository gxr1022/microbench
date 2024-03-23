CUR_DATE=("2024-02-20-09-33-32" "2024-02-20-11-01-27")
# cur_date="2024-02-20-09-33-32"
tmp_test_result="thread_compete_mem"
tmp_data="thread_compete_mem"

TEST_TYPE=("thread_compete_mem" "thread_bind_mem")
TMP_DATA_TYPE=("thread_compete_mem" "thread_bind_mem")
NUMA_NODE=(0 1 2 3)
TRAVERSE_TYPE=("rand" "seq")

l=${#TEST_TYPE[@]}
for ((j=0; j<l; j++)); do
    tmp_test_result=${TEST_TYPE[$j]}
    cur_date=${CUR_DATE[$j]}
    ./read2csv.sh $cur_date $tmp_test_result $tmp_test_result
    
    for traverse_type in "${TRAVERSE_TYPE[@]}"; do
        datafile1="/home/gxr/cxl_mem/microbench/data/$tmp_test_result/$cur_date/Calc.1.thd.numa0.log_$traverse_type.csv"
        datafile2="/home/gxr/cxl_mem/microbench/data/$tmp_test_result/$cur_date/Calc.1.thd.numa1.log_$traverse_type.csv"
        datafile3="/home/gxr/cxl_mem/microbench/data/$tmp_test_result/$cur_date/Calc.1.thd.numa2.log_$traverse_type.csv"
        datafile4="/home/gxr/cxl_mem/microbench/data/$tmp_test_result/$cur_date/Calc.1.thd.numa3.log_$traverse_type.csv"
        
        
        lat_output_dir="/home/gxr/cxl_mem/microbench/data/plot_graph/$tmp_test_result/single_latency/$cur_date"
        mkdir -p $lat_output_dir
        lat_output_png="single thread $traverse_type latency comparison_calc.png"
        
        bw_output_dir="/home/gxr/cxl_mem/microbench/data/plot_graph/$tmp_test_result/single_bandwidth/$cur_date"
        mkdir -p $bw_output_dir
        bw_output_png="single thread $traverse_type bandwidth comparison_calc.png"
       
        gnuplot -e "datafile1='$datafile1'" -e "datafile2='$datafile2'" -e "datafile3='$datafile3'" -e "datafile4='$datafile4'" -e "output_dir='$lat_output_dir'" -e "output_png='$lat_output_png'"  single_thd_lat_plot.plt
        gnuplot -e "datafile1='$datafile1'" -e "datafile2='$datafile2'" -e "datafile3='$datafile3'" -e "datafile4='$datafile4'" -e "output_dir='$bw_output_dir'" -e "output_png='$bw_output_png'"  single_thd_bw_plot.plt
        
    done

done






