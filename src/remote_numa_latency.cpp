#include <atomic>
#include <thread>
#include <iostream>
#include <vector>
#include <memory>

#include "../util/common.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include <vector>
#include <string>
#include <numa.h> 
#include "../util/affinity.hpp"

/*
available: 2 nodes (0-1)
node 0 cpus: 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47
node 0 size: 192052 MB
node 0 free: 132369 MB
node 1 cpus: 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 48 49 50 51 52 53 54 55 56 57 58 59 60 61 62 63
node 1 size: 192012 MB
node 1 free: 167987 MB
node distances:
node   0   1 
  0:  10  21 
  1:  21  10 
*/


/* Remote numa latency Test
 * 1 Thread
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
	// uint64_t offset_;
	// u8 hash = 0;


    Remote_numa_latency(){
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
            numa_free(base_addr_, capacity_);
            exit;
        }
		memset(base_addr_, 0, capacity_);	
        // memset(local_addr_, 0, BASIC_OP_POOL_SIZE);
	}
	

    void worker(void (*wr_method)(void *),bool seq, TraverseType trav_tp, FeedBackUnit *ret);
    void run(int core_id);
    void print_metrics(FeedBackUnit ret);

	~Remote_numa_latency(){
        numa_free(base_addr_, capacity_);
    }

};

void Remote_numa_latency::run(int core_id)
{
    cxl_mem::utils::set_affinity_relocate_stack(core_id);
    memset(&cl_buffer, 0, sizeof(cl_buffer));
    char *buf;

    for (int i = 0; i < BASIC_OPS_TASK_COUNT; i++)
    {
        
        if(FLAGS_traverse_type==false)
        {
            /*Sequential*/
            FeedBackUnit ret1;
            worker(bench_func[i],true,CALC_OFFSET,&ret1);
            print_metrics(ret1);

            /*Random*/
            FeedBackUnit ret3;
            worker(bench_func[i],false,CALC_OFFSET,&ret3);
            print_metrics(ret3);
        }
        else
        {
            FeedBackUnit ret2;
            worker(bench_func[i],true,CHASING_PTR,&ret2);
            print_metrics(ret2);

            FeedBackUnit ret4;
            worker(bench_func[i],false,CHASING_PTR,&ret4);
            print_metrics(ret4);
        }

        
    }
}

void Remote_numa_latency::print_metrics(FeedBackUnit ret)
{ 
    uint64_t lat = 0, bw=0;
    lat = ret.avg_latency;
    // bw =ret.avg_bandwidth*1000.0;
    bw = 64*1000.0/lat;
    
    std::cout << "-------result--------" << std::endl;
    std::cout << ret.work_type_ << " "
                << ret.traverse_type_ << " "
                << ret.order_ << std::endl;
    std::cout <<"[work set size]: ["
                << ret.wss_
                << "](bytes)" << std::endl;

    std::cout <<"[Latency]: ["
                << lat
                << "](ns)" << std::endl;
    // std::cout <<"[Bandwidth_ops]: ["
    //             << bw
    //             << "](ops)" << std::endl;
    std::cout <<"[Bandwidth_mbs]: ["
                << bw
                << "](MB/s)" << std::endl;
    std::cout << "---------------------" << std::endl;
        
}

/* run single type operation for many times, by accessing circular linked list*/
void Remote_numa_latency::worker(void (*wr_method)(void* ), bool seq, TraverseType trav_tp, FeedBackUnit *ret)
{
        access_unit_t *work_ptr = (access_unit_t *)base_addr_;
        auto unit_num = capacity_ / sizeof(access_unit_t); // define node numbers of linked list.
        std::vector<uint64_t> traverse_order(unit_num);
    
        // std::chrono::nanoseconds total_latency = std::chrono::nanoseconds(0);
        // uint64_t total_latency=0;
        
		/*define work type*/
		{
			if(trav_tp == CHASING_PTR)
            {
                ret->traverse_type_ = "[Trav]:[Ptr]";
            }
            else if (trav_tp == CALC_OFFSET)
            {
                ret->traverse_type_ = "[Trav]:[Calc]";
            }

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
            
        /*prepare for excuting benchmarks*/
        {
            // prepare order
            for (int i = 0; i < unit_num; i++)
            {
                traverse_order.at(i) = i;
            }

            if (!seq)
            {
                ret->order_ = "[order]:[rand]";
                std::random_shuffle(traverse_order.begin(), traverse_order.end());
            }
            else
            {
                ret->order_ = "[order]:[seq]";
            }
			/*create circular linked list*/
            for (int i = 1; i < unit_num; i++)
            {
                work_ptr[traverse_order.at(i - 1)].next = &work_ptr[traverse_order.at(i)];
            }
            work_ptr[traverse_order.at(unit_num - 1)].next = &work_ptr[traverse_order.at(0)];
            
        }

        /* run benchmarks*/
        {
            auto calc_lat = [&]()
            {
                // uint64_t start_tick, end_tick;
                // start_tick = rdtsc();
                // auto start_ns=std::chrono::high_resolution_clock::now();
                
                for (int i = 0; i < LATENCY_OPS_COUNT; i++)
                {
                    for (uint64_t j = 0; j < unit_num; j++)
                    {
                        auto start_ns=std::chrono::high_resolution_clock::now();

                        wr_method(work_ptr + traverse_order[j]);

                        auto end_ns = std::chrono::high_resolution_clock::now();
                        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end_ns - start_ns);
                        ret->total_latency += duration;
                        ret->wss_ +=64;
                        
                    }
                }
                // end_tick = rdtsc();
                // total_latency = end_tick - start_tick;
                // auto end_ns = std::chrono::high_resolution_clock::now();
                // auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end_ns - start_ns);
                // total_latency = duration;

                
            };

            auto chase_ptr_lat = [&]()
            {
                for (int i = 0; i < LATENCY_OPS_COUNT; i++)
                {
                    auto starting_point = work_ptr + random() % unit_num;
                    auto p = starting_point->next;
                    // uint64_t start_tick, end_tick;
                    // start_tick = rdtsc();
                    while (p!=nullptr && p != starting_point)
                    {
                        auto start_ns=std::chrono::high_resolution_clock::now();
                        wr_method(&p->pad[15]);
                        auto end_ns = std::chrono::high_resolution_clock::now();
                        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end_ns - start_ns);
                        ret->total_latency += duration;
                        ret->wss_ +=64;
                        
                        p = p->next;
                        if(!p)
                        {
                            std::cout<<work_ptr<<std::endl;
                        }
                    }
                    // end_tick = rdtsc();
                    // total_cycles += (end_tick - start_tick);
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

        ret->wss_ = capacity_;
        ret->avg_latency = (uint64_t)ret->total_latency.count() / LATENCY_OPS_COUNT / unit_num;
        ret->avg_bandwidth=unit_num*64/ret->total_latency.count()*1.0;
    };

int main(int argc, char **argv)
{
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    std::cout<<FLAGS_thread_number<<" threads excute memory test."<<std::endl;
    int threads_num=1;
    Remote_numa_latency test;
    std::vector<std::thread> local_threads;
    for (int i = 0; i < threads_num; i++){
        local_threads.push_back(std::thread(&Remote_numa_latency::run,&test,i));
    }
    for (int i = 0; i < threads_num; i++){
        local_threads[i].join();
    }
	return 0;

}
