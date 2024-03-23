#include <atomic>
#include <thread>
#include <iostream>
#include <vector>
#include <memory>
#include <mutex>

#include "../util/common.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include <vector>
#include <string>
#include <numa.h> 
#include "../util/affinity.hpp"
#include <chrono>
#include <iostream>

/*
gxr@test-H3C-UniServer-R4900-G6:~/cxl_mem/tool/MLC/Linux$ numactl --hardware
available: 2 nodes (0-1)
node 0 cpus: 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 48 49 50 51 52 53 54 55 56 57 58 59 60 61 62 63 64 65 66 67 68 69 70 71
node 0 size: 128645 MB
node 0 free: 123482 MB
node 1 cpus: 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47 72 73 74 75 76 77 78 79 80 81 82 83 84 85 86 87 88 89 90 91 92 93 94 95
node 1 size: 128953 MB
node 1 free: 119498 MB
node distances:
node   0   1 
  0:  10  21 
  1:  21  10 

*/

/*
gxr@test-H3C-UniServer-R4900-G6:~/cxl_mem/tool/MLC/Linux$ lscpu
架构：                   x86_64
  CPU 运行模式：         32-bit, 64-bit
  Address sizes:         52 bits physical, 57 bits virtual
  字节序：               Little Endian
CPU:                     96
  在线 CPU 列表：        0-95
厂商 ID：                GenuineIntel
  型号名称：             Intel(R) Xeon(R) Gold 6442Y
    CPU 系列：           6
    型号：               143
    每个核的线程数：     2
    每个座的核数：       24
    座：                 2
    步进：               7
    CPU 最大 MHz：       4000.0000
    CPU 最小 MHz：       800.0000
    BogoMIPS：           5200.00
    标记：               fpu vme de pse tsc msr pae mce cx8 apic sep mtrr pge mca cmov pat pse36 clflush dts acpi mmx fxsr sse sse2 ss h
                         t tm pbe syscall nx pdpe1gb rdtscp lm constant_tsc art arch_perfmon pebs bts rep_good nopl xtopology nonstop_ts
                         c cpuid aperfmperf tsc_known_freq pni pclmulqdq dtes64 monitor ds_cpl vmx smx est tm2 ssse3 sdbg fma cx16 xtpr 
                         pdcm pcid dca sse4_1 sse4_2 x2apic movbe popcnt tsc_deadline_timer aes xsave avx f16c rdrand lahf_lm abm 3dnowp
                         refetch cpuid_fault epb cat_l3 cat_l2 cdp_l3 invpcid_single intel_ppin cdp_l2 ssbd mba ibrs ibpb stibp ibrs_enh
                         anced tpr_shadow vnmi flexpriority ept vpid ept_ad fsgsbase tsc_adjust bmi1 avx2 smep bmi2 erms invpcid cqm rdt
                         _a avx512f avx512dq rdseed adx smap avx512ifma clflushopt clwb intel_pt avx512cd sha_ni avx512bw avx512vl xsave
                         opt xsavec xgetbv1 xsaves cqm_llc cqm_occup_llc cqm_mbm_total cqm_mbm_local split_lock_detect avx_vnni avx512_b
                         f16 wbnoinvd dtherm ida arat pln pts hwp hwp_act_window hwp_epp hwp_pkg_req hfi avx512vbmi umip pku ospke waitp
                         kg avx512_vbmi2 gfni vaes vpclmulqdq avx512_vnni avx512_bitalg tme avx512_vpopcntdq la57 rdpid bus_lock_detect 
                         cldemote movdiri movdir64b enqcmd fsrm md_clear serialize tsxldtrk pconfig arch_lbr ibt amx_bf16 avx512_fp16 am
                         x_tile amx_int8 flush_l1d arch_capabilities
Virtualization features: 
  虚拟化：               VT-x
Caches (sum of all):     
  L1d:                   2.3 MiB (48 instances)
  L1i:                   1.5 MiB (48 instances)
  L2:                    96 MiB (48 instances)
  L3:                    120 MiB (2 instances)
NUMA:                    
  NUMA 节点：            2
  NUMA 节点0 CPU：       0-23,48-71
  NUMA 节点1 CPU：       24-47,72-95
*/

/* Remote numa latency Test
 * multiply threads: we don't bind thread on specific remote memory area. Every thread has equal opportunity to compete resources by * * mutex lock.
 * Run a series of microbenchmarks on combination of store, flush and fence operations.
 * Sequential -> Random
 */
class Remote_numa_latency
{
private:
	/* data */
public:
	char *base_addr_; 
    // char *local_addr_;
	uint64_t capacity_;
    uint64_t threads_num;
	// uint64_t offset_;
	// u8 hash = 0;
    std::mutex mutex; // 互斥锁
    uint64_t index_counter = 0; // 全局索引计数器
    access_unit_t *ptr_chase; 


    Remote_numa_latency(){
        threads_num= FLAGS_thread_number;
		// init memory pool on NUMA1, thread 0 on NUMA0 and load/store data on NUMA1.
		capacity_=1<<FLAGS_pool_bits;
		base_addr_ = (char*)numa_alloc_onnode(1<<FLAGS_pool_bits, FLAGS_numa_node);
        if(FLAGS_numa_node==0)
        {
            std::cout<<"Starting local DRAM test!"<<std::endl; 
        }
        else{
            std::cout<<"Starting remote numa DRAM test!"<<std::endl; 
        }
        // local_addr_ = (char*)numa_alloc_onnode(BASIC_OP_POOL_SIZE, 0);//给numa0分配1MB内存
        if (((uint64_t)base_addr_ % ALIGNMENT) != 0){
            std::cout << "share_mem is not aligned to 64 bytes" << std::endl;
            numa_free(base_addr_, 1<<FLAGS_pool_bits);
            exit;
        }
		memset(base_addr_, 0, 1<<FLAGS_pool_bits);	
        ptr_chase = (access_unit_t *)base_addr_;
        // memset(local_addr_, 0, BASIC_OP_POOL_SIZE);
	}
	

    void run_one_type(bool seq, TraverseType trav_tp);
    void worker(void (*wr_method)(void *), TraverseType trav_tp, FeedBackUnit *ret, int core_id, std::vector<uint64_t> traverse_order, uint64_t unit_num, access_unit_t *work_ptr);
    void run();
    void print_metrics(FeedBackUnit ret);

	~Remote_numa_latency(){
        numa_free(base_addr_, 1<<FLAGS_pool_bits);
    }

};

void Remote_numa_latency::run_one_type(bool seq, TraverseType trav_tp)
{
                
    for (int k = 0; k < BASIC_OPS_TASK_COUNT; k++)
    { 
        memset(&cl_buffer, 0, sizeof(cl_buffer));
        FeedBackUnit ret;
        auto unit_num = capacity_ / sizeof(access_unit_t); // define node numbers of linked list.
        std::vector<uint64_t> traverse_order(unit_num);
        access_unit_t *work_ptr = (access_unit_t *)base_addr_;

        // ret.wss_ = capacity_;
        /*define work type*/
		{
			if(trav_tp == CHASING_PTR)
            {
                ret.traverse_type_ = "[Trav]:[Ptr]";
            }
            else if (trav_tp == CALC_OFFSET)
            {
                ret.traverse_type_ = "[Trav]:[Calc]";
            }

		}
            
        /*prepare for excuting benchmarks*/
        {
            // prepare order
            for (int i = 0; i < unit_num; i++)
            {
                traverse_order.at(i) = i;
            }

            if (!seq)
            {
                ret.order_ = "[order]:[rand]";
                std::random_shuffle(traverse_order.begin(), traverse_order.end());
            }
            else
            {
                ret.order_ = "[order]:[seq]";
            }
			/*create circular linked list*/
            for (int i = 1; i < unit_num; i++)
            {
                work_ptr[traverse_order.at(i - 1)].next = &work_ptr[traverse_order.at(i)];
            }
            work_ptr[traverse_order.at(unit_num - 1)].next = &work_ptr[traverse_order.at(0)];
            
        }


        /*global variables*/
        ptr_chase = work_ptr->next;

        std::vector<std::thread> local_threads;
        // std::vector<int> threads_mapping={0, 1 ,2 ,3, 4, 5, 6 ,7, 8 ,9, 10, 11, 24, 25, 26, 27, 28 ,29, 30,31, 32 ,33, 34, 35};
        std::vector<int> threads_mapping={12, 13 ,14 ,15, 16, 17 ,18 ,19, 20, 21 ,22, 23 ,36 ,37, 38 ,39, 40 ,41 ,42 ,43, 44, 45, 46 ,47};

        for (int i = 0; i < threads_num; i++){
            local_threads.push_back(std::thread(
                [this, &ret, k,i, threads_mapping, seq, trav_tp,traverse_order, unit_num, work_ptr]() {
                    this->worker(bench_func[k], trav_tp, &ret,threads_mapping[i],traverse_order, unit_num, work_ptr);
                }));
        }
        for (int i = 0; i < threads_num; i++){
            local_threads[i].join();
        }
        /*平均每个操作的延迟 & 多个线程总带宽*/
        // ret.avg_latency = (uint64_t)total_latency.count() / LATENCY_OPS_COUNT /unit_num/ threads_num; //用所有访问操作的总延迟/访问操作数/线程数目
        ret.avg_latency = (uint64_t)ret.total_latency.count() / LATENCY_OPS_COUNT /unit_num/ threads_num;
        
        ret.avg_bandwidth = 1000000000.0/ret.avg_latency; 
        print_metrics(ret);
        
    }
}

void Remote_numa_latency::run()
{
    if(FLAGS_traverse_type==false)
    {
        run_one_type(true,CALC_OFFSET);
        run_one_type(false,CALC_OFFSET);
    }
    else 
    {
        run_one_type(true,CHASING_PTR);
        run_one_type(false,CHASING_PTR);
    }
    
    
}

void Remote_numa_latency::print_metrics(FeedBackUnit ret)
{ 
    uint64_t lat;
    u_int64_t bw , bw_MB;
    lat = ret.avg_latency;
    bw = ret.avg_bandwidth ; 
    bw_MB = bw * 64 / 1000000.0;

    
    std::cout << "-------result--------" << std::endl;
    std::cout << ret.work_type_ << " "
                << ret.traverse_type_ << " "
                << ret.order_ << std::endl;
    std::cout <<"[work set size]: ["
                << ret.wss_
                << "](bytes)" << std::endl;
    std::cout <<"[unit num]: ["
                << capacity_/sizeof(access_unit_t)
                << "](op)" << std::endl;
    std::cout <<"[Latency]: ["
                << lat
                << "](ns)" << std::endl;
    std::cout <<"[Bandwidth_ops]: ["
                << bw
                << "](ops)" << std::endl;
    std::cout <<"[Bandwidth_mbs]: ["
                << bw_MB
                << "](MB/s)" << std::endl;
    std::cout << "---------------------" << std::endl;
        
}

/* run single type operation for many times, by accessing circular linked list*/
void Remote_numa_latency::worker(void (*wr_method)(void* ), TraverseType trav_tp, FeedBackUnit *ret, int core_id, std::vector<uint64_t> traverse_order, uint64_t unit_num, access_unit_t *work_ptr)
{
        cxl_mem::utils::set_affinity_relocate_stack(core_id);  // set CPU affinity
        index_counter=0;
        
        // Define operation type, such as clwb, nt-store ...
        {
            if (wr_method == clf_load)
            {
                ret->work_type_ = "[type]:[clf_load]";
            }
            else if (wr_method == load)
            {
                ret->work_type_ = "[type]:[load]";
            }
            else if (wr_method == wr_nt)
            {
                ret->work_type_ = "[type]:[nt store]";
            }
            else if (wr_method == wr_clwb)
            {
                ret->work_type_ = "[type]:[clwb]";
            }
            else if (wr_method == wr_clwb_sfence)
            {
                ret->work_type_ = "[type]:[clwb sfence]";
            }
            else if (wr_method == wr_nt_sfence)
            {
                ret->work_type_ = "[type]:[nt store sfence]";
            }
        
		}

        /* run benchmarks*/
        {
            auto calc_lat = [&]()
            {
                // uint64_t start_tick, end_tick;
                for (int i = 0; i < LATENCY_OPS_COUNT; i++)
                {
                    
                        while(true) 
                        {
                            std::unique_lock<std::mutex> lock(mutex);

                            // start_tick = rdtsc();
                            if(index_counter>=unit_num) 
                            {
                                break;
                            }
                            
                            auto start_ns=std::chrono::high_resolution_clock::now();
                            wr_method(work_ptr + traverse_order[index_counter]);
                            // end_tick = rdtsc(); 
                            auto end_ns = std::chrono::high_resolution_clock::now();
                            auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end_ns - start_ns);
                            ret->total_latency += duration;
                            // std::cout<<total_latency.count()<<std::endl;
                            
                            
                            index_counter++;
                            ret->wss_+=64;
                            lock.unlock();
                            
                        }   
                }
            };

            auto chase_ptr_lat = [&]()
            {
                for (int i = 0; i < LATENCY_OPS_COUNT; i++)
                {
                    // uint64_t thread_wss = ROUND_DOWN(wss / threa, 64ULL);
                    while(true) 
                    {
                        std::unique_lock<std::mutex> lock(mutex);

                        if(!ptr_chase || ptr_chase == work_ptr)
                        {
                            break;
                        }
                        auto start_ns=std::chrono::high_resolution_clock::now();

                        wr_method(&ptr_chase->pad[15]);

                        auto end_ns = std::chrono::high_resolution_clock::now();
                        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end_ns - start_ns);
                        ret->total_latency += duration;
                        ret->wss_+=64;
                        ptr_chase = ptr_chase->next;
                        
                        lock.unlock();
                    }
                 
                }
            };
            
            if(trav_tp == CHASING_PTR)
            {
                chase_ptr_lat();
            }
            else if (trav_tp == CALC_OFFSET)
            {
                calc_lat();
            }
        }
    };

int main(int argc, char **argv)
{
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    std::cout<<FLAGS_thread_number<<" threads excute memory test."<<std::endl;
    Remote_numa_latency test;
    test.run();
    
	return 0;

}
