# cd '/home/gxr/cxl_mem/microbench/data/plot_graph/thread_bind_mem/bandwidth' # 更改到输出文件夹位置
# cd '/home/gxr/cxl_mem/microbench/data/plot_graph/thread_bind_mem/bandwidth' # 更改到输出文件夹位置
# cd '/home/gxr/cxl_mem/microbench/data/plot_graph/single_thread_test/bandwidth' # 更改到输出文件夹位置
# set output "single thread sequential  bandwidth comparison_256_calc.png" # 设置输出文件名
# set output "single thread sequential  bandwidth comparison_256_ptr.png" # 设置输出文件名
# set output "single thread random  bandwidth comparison_256_calc_1130_.png" # 设置输出文件名

cd output_dir # 更改到输出文件夹位置
set terminal pngcairo enhanced font "arial,10" # 设置输出文件类型为PNG
set output output_png # 设置输出文件名


set style data histograms # 设置数据样式为柱状图

set style histogram cluster gap 1 # 设置柱状图的样式
set style fill solid border -1

set offsets 0, 0, 0, 0  # 顶部、底部、左侧和右侧边距都设置为0


set title "Bandwidth vs Type"
set xlabel "Operation types" # 设置X轴标签
set ylabel "Bandwidth(MB/s)" # 设置Y轴标签

set xtic rotate by -45 scale 0 font ",12"
set xtics 2.0 

# set boxwidth 2 absolute   # 设置箱子宽度为0.5单位

set key outside # 设置图例位置

set datafile separator ","

set style data histogram

plot \
    datafile1 every ::1::999 using 6:xtic(1) title "DDR-L" lc rgb "blue" ,\
    datafile2  every ::1::999 using 6:xtic(1) title "DDR-R" lc rgb "red" ,\
    datafile3  every ::1::999 using 6:xtic(1) title "CXL-R" lc rgb "green" ,\
    datafile4  every ::1::999 using 4:xtic(1) title "CXL-L" lc rgb "orange"

