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
CMAKE_SOURCE_DIR = /home/newcoder/project/mutex

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/newcoder/project/mutex/build/arm

# Include any dependencies generated for this target.
include modules/serial/test/CMakeFiles/serial_test.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include modules/serial/test/CMakeFiles/serial_test.dir/compiler_depend.make

# Include the progress variables for this target.
include modules/serial/test/CMakeFiles/serial_test.dir/progress.make

# Include the compile flags for this target's objects.
include modules/serial/test/CMakeFiles/serial_test.dir/flags.make

modules/serial/test/CMakeFiles/serial_test.dir/src/main.cpp.o: modules/serial/test/CMakeFiles/serial_test.dir/flags.make
modules/serial/test/CMakeFiles/serial_test.dir/src/main.cpp.o: ../../modules/serial/test/src/main.cpp
modules/serial/test/CMakeFiles/serial_test.dir/src/main.cpp.o: modules/serial/test/CMakeFiles/serial_test.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/newcoder/project/mutex/build/arm/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object modules/serial/test/CMakeFiles/serial_test.dir/src/main.cpp.o"
	cd /home/newcoder/project/mutex/build/arm/modules/serial/test && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT modules/serial/test/CMakeFiles/serial_test.dir/src/main.cpp.o -MF CMakeFiles/serial_test.dir/src/main.cpp.o.d -o CMakeFiles/serial_test.dir/src/main.cpp.o -c /home/newcoder/project/mutex/modules/serial/test/src/main.cpp

modules/serial/test/CMakeFiles/serial_test.dir/src/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/serial_test.dir/src/main.cpp.i"
	cd /home/newcoder/project/mutex/build/arm/modules/serial/test && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/newcoder/project/mutex/modules/serial/test/src/main.cpp > CMakeFiles/serial_test.dir/src/main.cpp.i

modules/serial/test/CMakeFiles/serial_test.dir/src/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/serial_test.dir/src/main.cpp.s"
	cd /home/newcoder/project/mutex/build/arm/modules/serial/test && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/newcoder/project/mutex/modules/serial/test/src/main.cpp -o CMakeFiles/serial_test.dir/src/main.cpp.s

# Object files for target serial_test
serial_test_OBJECTS = \
"CMakeFiles/serial_test.dir/src/main.cpp.o"

# External object files for target serial_test
serial_test_EXTERNAL_OBJECTS =

modules/serial/test/serial_test: modules/serial/test/CMakeFiles/serial_test.dir/src/main.cpp.o
modules/serial/test/serial_test: modules/serial/test/CMakeFiles/serial_test.dir/build.make
modules/serial/test/serial_test: modules/serial/test/CMakeFiles/serial_test.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/newcoder/project/mutex/build/arm/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable serial_test"
	cd /home/newcoder/project/mutex/build/arm/modules/serial/test && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/serial_test.dir/link.txt --verbose=$(VERBOSE)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold "Installing and stripping serial_test"
	cd /home/newcoder/project/mutex/build/arm/modules/serial/test && ./serial_test
	cd /home/newcoder/project/mutex/build/arm/modules/serial/test && /usr/bin/cmake --build . --target install

# Rule to build all files generated by this target.
modules/serial/test/CMakeFiles/serial_test.dir/build: modules/serial/test/serial_test
.PHONY : modules/serial/test/CMakeFiles/serial_test.dir/build

modules/serial/test/CMakeFiles/serial_test.dir/clean:
	cd /home/newcoder/project/mutex/build/arm/modules/serial/test && $(CMAKE_COMMAND) -P CMakeFiles/serial_test.dir/cmake_clean.cmake
.PHONY : modules/serial/test/CMakeFiles/serial_test.dir/clean

modules/serial/test/CMakeFiles/serial_test.dir/depend:
	cd /home/newcoder/project/mutex/build/arm && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/newcoder/project/mutex /home/newcoder/project/mutex/modules/serial/test /home/newcoder/project/mutex/build/arm /home/newcoder/project/mutex/build/arm/modules/serial/test /home/newcoder/project/mutex/build/arm/modules/serial/test/CMakeFiles/serial_test.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : modules/serial/test/CMakeFiles/serial_test.dir/depend

