# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/gxr/cxl_mem/microbench/src

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/gxr/cxl_mem/microbench/src/build

# Include any dependencies generated for this target.
include CMakeFiles/LatencyTest.exec.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/LatencyTest.exec.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/LatencyTest.exec.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/LatencyTest.exec.dir/flags.make

CMakeFiles/LatencyTest.exec.dir/multi_thread_wss_numa_latency.o: CMakeFiles/LatencyTest.exec.dir/flags.make
CMakeFiles/LatencyTest.exec.dir/multi_thread_wss_numa_latency.o: ../multi_thread_wss_numa_latency.cpp
CMakeFiles/LatencyTest.exec.dir/multi_thread_wss_numa_latency.o: CMakeFiles/LatencyTest.exec.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/gxr/cxl_mem/microbench/src/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/LatencyTest.exec.dir/multi_thread_wss_numa_latency.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/LatencyTest.exec.dir/multi_thread_wss_numa_latency.o -MF CMakeFiles/LatencyTest.exec.dir/multi_thread_wss_numa_latency.o.d -o CMakeFiles/LatencyTest.exec.dir/multi_thread_wss_numa_latency.o -c /home/gxr/cxl_mem/microbench/src/multi_thread_wss_numa_latency.cpp

CMakeFiles/LatencyTest.exec.dir/multi_thread_wss_numa_latency.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/LatencyTest.exec.dir/multi_thread_wss_numa_latency.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/gxr/cxl_mem/microbench/src/multi_thread_wss_numa_latency.cpp > CMakeFiles/LatencyTest.exec.dir/multi_thread_wss_numa_latency.i

CMakeFiles/LatencyTest.exec.dir/multi_thread_wss_numa_latency.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/LatencyTest.exec.dir/multi_thread_wss_numa_latency.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/gxr/cxl_mem/microbench/src/multi_thread_wss_numa_latency.cpp -o CMakeFiles/LatencyTest.exec.dir/multi_thread_wss_numa_latency.s

# Object files for target LatencyTest.exec
LatencyTest_exec_OBJECTS = \
"CMakeFiles/LatencyTest.exec.dir/multi_thread_wss_numa_latency.o"

# External object files for target LatencyTest.exec
LatencyTest_exec_EXTERNAL_OBJECTS =

LatencyTest.exec: CMakeFiles/LatencyTest.exec.dir/multi_thread_wss_numa_latency.o
LatencyTest.exec: CMakeFiles/LatencyTest.exec.dir/build.make
LatencyTest.exec: /usr/local/lib/libgflags.a
LatencyTest.exec: CMakeFiles/LatencyTest.exec.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/gxr/cxl_mem/microbench/src/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable LatencyTest.exec"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/LatencyTest.exec.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/LatencyTest.exec.dir/build: LatencyTest.exec
.PHONY : CMakeFiles/LatencyTest.exec.dir/build

CMakeFiles/LatencyTest.exec.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/LatencyTest.exec.dir/cmake_clean.cmake
.PHONY : CMakeFiles/LatencyTest.exec.dir/clean

CMakeFiles/LatencyTest.exec.dir/depend:
	cd /home/gxr/cxl_mem/microbench/src/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/gxr/cxl_mem/microbench/src /home/gxr/cxl_mem/microbench/src /home/gxr/cxl_mem/microbench/src/build /home/gxr/cxl_mem/microbench/src/build /home/gxr/cxl_mem/microbench/src/build/CMakeFiles/LatencyTest.exec.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/LatencyTest.exec.dir/depend

