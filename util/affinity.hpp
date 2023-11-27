#pragma once
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <numa.h>
#include <numaif.h>
#include <unistd.h>

#include <array>
#include <iostream>
#include <memory>

namespace cxl_mem {

namespace utils {

//设置线程的CPU亲和性，让线程在特定的CPU核心上运行
void set_affinity(uint64_t i) {
  cpu_set_t mask;
  CPU_ZERO(&mask);
  CPU_SET(i, &mask); //将第 i 位设置为1，表示设置线程将在对应的 CPU 核心上运行
  if (sched_setaffinity(0, sizeof(cpu_set_t), &mask) == -1) { //用于设置当前线程的 CPU 亲和性
    std::cout << "sched_setaffinity" << std::endl;
    assert(false);
  }
}


// https://stackoverflow.com/questions/10605766/allocating-a-threads-stack-on-a-specific-numa-memory
void* PreFaultStack()
{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-local-addr"
    const size_t NUM_PAGES_TO_PRE_FAULT = 50;
    const size_t size = NUM_PAGES_TO_PRE_FAULT * numa_pagesize();
    void *allocaBase = alloca(size);
    memset(allocaBase, 0, size);
    return allocaBase;
#pragma GCC diagnostic pop
}

void set_affinity_relocate_stack(int i) {
  cpu_set_t mask;
  assert(-1 != i);
  CPU_ZERO(&mask);
  CPU_SET(i, &mask);
  if (sched_setaffinity(0, sizeof(cpu_set_t), &mask) == -1) {
    std::cout << "sched_setaffinity" << std::endl;
    assert(false);
  }
  pthread_attr_t attr;
  void *stackAddr = nullptr;
  size_t stackSize = 0;
  if ((0 != pthread_getattr_np(pthread_self(), &attr)) ||
      (0 != pthread_attr_getstack(&attr, &stackAddr, &stackSize))) {
    assert(false);
  }
  const unsigned long nodeMask = 1UL << numa_node_of_cpu(i); //获取指定CPU核心所在的NUMA节点
  const auto bindRc = mbind(stackAddr, stackSize, MPOL_BIND, &nodeMask, sizeof(nodeMask), MPOL_MF_MOVE | MPOL_MF_STRICT); //将进程的内存页绑定到特定的内存节点,（NUMA 节点）上，这样便可以给指定线程分配特定NUMA节点的空间。
  assert(0 == bindRc);

  PreFaultStack();
}



void print_affinity() {
  cpu_set_t mask;
  long nproc, i;

  if (sched_getaffinity(0, sizeof(cpu_set_t), &mask) == -1) {
    perror("sched_getaffinity");
    assert(false);
  }
  nproc = sysconf(_SC_NPROCESSORS_ONLN);
  printf("sched_getaffinity = ");
  for (i = 0; i < nproc; i++) {
    printf("%d ", CPU_ISSET(i, &mask));
  }
  printf("\n");
}

} // namespace utils

} // namespace cxl_mem