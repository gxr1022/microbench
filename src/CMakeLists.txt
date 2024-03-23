option(ENABLE_DEBUG "Enable Debug Mode" ON)
if(ENABLE_DEBUG)
    set(CMAKE_BUILD_TYPE Debug)
else()
    set(CMAKE_BUILD_TYPE Release) 
endif()

find_package(gflags REQUIRED)

# add_executable(LatencyTest.exec
# remote_numa_latency.cpp ../util/common.h ../util/affinity.hpp
# )

# add_executable(LatencyTest.exec
# multi_thread_numa_latency.cpp ../util/common.h ../util/affinity.hpp
# )

add_executable(LatencyTest.exec
multi_thread_wss_numa_latency.cpp ../util/common.h ../util/affinity.hpp ../util/barrier.hpp
)

target_link_libraries(LatencyTest.exec
PRIVATE
numa ${GFLAGS_LIBRARIES}
)

include_directories(${GFLAGS_INCLUDE_DIRS})

target_compile_options(LatencyTest.exec
PRIVATE
-fpermissive 
)

set(CMAKE_CXX_FLAGS "-march=native")
add_definitions(-w)


if(ENABLE_DEBUG)
    message("Adding debug flags...")
    target_compile_options(LatencyTest.exec PUBLIC -g) # 添加调试信息
endif()