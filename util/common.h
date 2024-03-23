#pragma once
#include <x86intrin.h>
#include <algorithm>
#include <fcntl.h>
#include <unistd.h>
#include <gflags/gflags.h>

inline uint64_t rdtsc()
{
    unsigned int lo, hi;
    __asm__ __volatile__("rdtsc"
                         : "=a"(lo), "=d"(hi));
    return ((uint64_t)hi << 32) | lo;
}

#define NPAD 31 /*padding the size of the whole struct*/
#define ALIGNMENT 64

#define LATENCY_OPS_COUNT 1L
#define BASIC_OPS_TASK_COUNT 6 /*不同的访存策略种类，比如store, nt-store……*/
#define PAGE_SHIFT 12

#define BASIC_OP_POOL_PAGE_BIT (BASIC_OP_POOL_BITS - PAGE_SHIFT)
#define BASIC_OP_POOL_PAGES (1L << BASIC_OP_POOL_PAGE_BIT) // pages
#define BASIC_OP_MASK 0x3FFFFFC0                           /*0b1{24, POOL_LINE_BITS}0{6, CACHELINE_BITS} */


#define ROUND_UP(x, y) (((x) + (y - 1)) & ~(y - 1))
#define ROUND_DOWN(x, y) ((x) & ~(y - 1))

DEFINE_uint64(thread_number, 1, "Define thread numbers");
DEFINE_uint64(numa_node, 0, "Define NUMA node id");
DEFINE_uint64(pool_bits, 30, "Define memory pool bits");
DEFINE_bool(traverse_type, false, "Define traverse type: CHASING_PTR or CALC_OFFSET ");
// DEFINE_uint64(ops_task_count,6,"Define operation types");
// DEFINE_uint64(npad,7,"padding the size of the whole struct, default 64 bytes");

struct access_unit_t /* the size of access_unit_t is 64 bytes: 8*7+8=64, which is the same as cacheline */
{
    struct access_unit_t *next;
    uint64_t pad[NPAD];
};

enum TraverseType
{
    CHASING_PTR,
    CALC_OFFSET
};

struct FeedBackUnit
{
    std::string work_type_;
    std::string order_;
    std::string traverse_type_;
    uint64_t wss_ = 0, avg_latency = 0, avg_bandwidth = 0;
    std::chrono::nanoseconds total_latency = std::chrono::nanoseconds(0);
};

static __m512i cl_buffer;
static __m512i loaded_data;

void wr_clwb_sfence(void *addr)
{
    *((int *)addr) += 10;
    _mm_clwb(addr);
    _mm_sfence();
}

void wr_nt_sfence(void *addr)
{
    /*    if (((uintptr_t)addr % ALIGNMENT) != 0) {
           fprintf(stderr, "Error: Memory not aligned to %d bytes\n", ALIGNMENT);
           return EXIT_FAILURE;
       } */
    _mm512_stream_si512((__m512i *)addr, cl_buffer); 
    _mm_sfence();
}

void wr_clwb(void *addr)
{
    *((int *)addr) += 10;
    _mm_clwb(addr);
}

void wr_nt(void *addr)
{
    _mm512_stream_si512((__m512i *)addr, cl_buffer);
}

void clf_load(char *src)
{

    _mm_clflush(src);
    loaded_data = _mm512_load_si512((__m512i *)src);
    _mm_sfence();
}

void load(char *src)
{

    loaded_data = _mm512_load_si512((__m512i *)src);
    _mm_sfence();
}

void (*bench_func[BASIC_OPS_TASK_COUNT])(void *) = {
    &wr_clwb_sfence,
    &wr_clwb,
    &wr_nt_sfence,
    &wr_nt,
    &clf_load,
    &load
};

void get_random_bytes(void *buf, int nbytes)
{
    int fd = open("/dev/urandom", O_RDONLY);
    if (fd < 0)
    {
        perror("open");
        exit(EXIT_FAILURE);
    }

    ssize_t result = read(fd, buf, nbytes);
    if (result < 0)
    {
        perror("read");
        close(fd);
        exit(EXIT_FAILURE);
    }

    close(fd);
}

double cpucycles_to_ns(uint64_t cpu_cycles)
{

    double clockRateGHz = 3.0;

    // Transfer to ns.
    double ns = (static_cast<double>(cpu_cycles) / clockRateGHz) * 1000; // 转换为纳秒

    return ns;
}
