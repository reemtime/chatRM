# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.25

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
CMAKE_SOURCE_DIR = /home/night/Desktop/project/chatRM/server

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/night/Desktop/project/chatRM/server/build

# Include any dependencies generated for this target.
include lib/libdb/CMakeFiles/dbrequests.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include lib/libdb/CMakeFiles/dbrequests.dir/compiler_depend.make

# Include the progress variables for this target.
include lib/libdb/CMakeFiles/dbrequests.dir/progress.make

# Include the compile flags for this target's objects.
include lib/libdb/CMakeFiles/dbrequests.dir/flags.make

lib/libdb/CMakeFiles/dbrequests.dir/src/db.cpp.o: lib/libdb/CMakeFiles/dbrequests.dir/flags.make
lib/libdb/CMakeFiles/dbrequests.dir/src/db.cpp.o: /home/night/Desktop/project/chatRM/server/lib/libdb/src/db.cpp
lib/libdb/CMakeFiles/dbrequests.dir/src/db.cpp.o: lib/libdb/CMakeFiles/dbrequests.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/night/Desktop/project/chatRM/server/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object lib/libdb/CMakeFiles/dbrequests.dir/src/db.cpp.o"
	cd /home/night/Desktop/project/chatRM/server/build/lib/libdb && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT lib/libdb/CMakeFiles/dbrequests.dir/src/db.cpp.o -MF CMakeFiles/dbrequests.dir/src/db.cpp.o.d -o CMakeFiles/dbrequests.dir/src/db.cpp.o -c /home/night/Desktop/project/chatRM/server/lib/libdb/src/db.cpp

lib/libdb/CMakeFiles/dbrequests.dir/src/db.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/dbrequests.dir/src/db.cpp.i"
	cd /home/night/Desktop/project/chatRM/server/build/lib/libdb && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/night/Desktop/project/chatRM/server/lib/libdb/src/db.cpp > CMakeFiles/dbrequests.dir/src/db.cpp.i

lib/libdb/CMakeFiles/dbrequests.dir/src/db.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/dbrequests.dir/src/db.cpp.s"
	cd /home/night/Desktop/project/chatRM/server/build/lib/libdb && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/night/Desktop/project/chatRM/server/lib/libdb/src/db.cpp -o CMakeFiles/dbrequests.dir/src/db.cpp.s

# Object files for target dbrequests
dbrequests_OBJECTS = \
"CMakeFiles/dbrequests.dir/src/db.cpp.o"

# External object files for target dbrequests
dbrequests_EXTERNAL_OBJECTS =

lib/libdb/libdbrequests.a: lib/libdb/CMakeFiles/dbrequests.dir/src/db.cpp.o
lib/libdb/libdbrequests.a: lib/libdb/CMakeFiles/dbrequests.dir/build.make
lib/libdb/libdbrequests.a: lib/libdb/CMakeFiles/dbrequests.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/night/Desktop/project/chatRM/server/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX static library libdbrequests.a"
	cd /home/night/Desktop/project/chatRM/server/build/lib/libdb && $(CMAKE_COMMAND) -P CMakeFiles/dbrequests.dir/cmake_clean_target.cmake
	cd /home/night/Desktop/project/chatRM/server/build/lib/libdb && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/dbrequests.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
lib/libdb/CMakeFiles/dbrequests.dir/build: lib/libdb/libdbrequests.a
.PHONY : lib/libdb/CMakeFiles/dbrequests.dir/build

lib/libdb/CMakeFiles/dbrequests.dir/clean:
	cd /home/night/Desktop/project/chatRM/server/build/lib/libdb && $(CMAKE_COMMAND) -P CMakeFiles/dbrequests.dir/cmake_clean.cmake
.PHONY : lib/libdb/CMakeFiles/dbrequests.dir/clean

lib/libdb/CMakeFiles/dbrequests.dir/depend:
	cd /home/night/Desktop/project/chatRM/server/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/night/Desktop/project/chatRM/server /home/night/Desktop/project/chatRM/server/lib/libdb /home/night/Desktop/project/chatRM/server/build /home/night/Desktop/project/chatRM/server/build/lib/libdb /home/night/Desktop/project/chatRM/server/build/lib/libdb/CMakeFiles/dbrequests.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : lib/libdb/CMakeFiles/dbrequests.dir/depend

