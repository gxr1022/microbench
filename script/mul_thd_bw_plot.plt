# cd '/home/gxr/cxl_mem/microbench/data/plot_graph/thread_bind_mem/bandwidth' # 更改到输出文件夹位置

cd output_dir # 更改到输出文件夹位置
set terminal pngcairo enhanced font "arial,10" # 设置输出文件类型为PNG

# set output "multiply threads remote sequential bandwidth comparison_256_calc.png" # 设置输出文件名
set output output_png # 设置输出文件名

set style data lines # 设置数据样式为柱状图

set style histogram cluster gap 1 # 设置柱状图的样式
set style fill solid border -1

set offsets 0, 0, 0, 0  # 顶部、底部、左侧和右侧边距都设置为0


set title "Bandwidth vs Type"
set xlabel "Operation types" # 设置X轴标签
set ylabel "Bandwidth(MB/s)" # 设置Y轴标签

set xtic rotate by -45 scale 0 font ",12"
# set xtics 2.0 
#set xrange [1:20]


set key outside # 设置图例位置



set key autotitle columnhead # 设置图例


# set datafile separator "\t" # 设置数据分隔符

plot datafile using 1:2 title "clwb sfence" with lines lw 2 linecolor rgb "blue",\
     ''   using 1:3 title "clwb" with lines lw 2 linecolor rgb "red",\
     ''  using 1:4 title "nt sfence" with lines lw 2 linecolor rgb "green",\
     ''  using 1:5 title "nt-store" with lines lw 2 linecolor rgb "orange",\
     ''   using 1:6 title "clf-load" with lines lw 2 linecolor rgb "purple",\
     ''  using 1:7 title "load" with lines lw 2 linecolor rgb "cyan" 
