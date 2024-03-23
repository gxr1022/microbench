# sort -n -k1 compete_remote_seq_bandwidth_output.dat > sorted_compete_remote_seq_bandwidth_output.dat
# sort -n -k1 compete_local_seq_bandwidth_output.dat > sorted_compete_local_seq_bandwidth_output.dat

# sort -n -k1 compete_local_rand_bandwidth_output.dat > sorted_compete_local_rand_bandwidth_output.dat

cur_date=$1
bandwidth_data=$2
sorted_csv_folder="/home/gxr/cxl_mem/microbench/data/sorted_data/$cur_date"
merged_csv_folder="/home/gxr/cxl_mem/microbench/data/merged_data/$cur_date"
mkdir -p "$sorted_csv_folder"

merged_dat_file="$merged_csv_folder/$bandwidth_data"
sorted_dat_file="$sorted_csv_folder/$bandwidth_data"

sort -n -k1 $merged_dat_file > $sorted_dat_file