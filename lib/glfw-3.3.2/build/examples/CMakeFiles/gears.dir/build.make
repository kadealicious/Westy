# CMAKE generated file: DO NOT EDIT!
# Generated by "MinGW Makefiles" Generator, CMake Version 3.19

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

SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = C:\CMake\bin\cmake.exe

# The command to remove a file.
RM = C:\CMake\bin\cmake.exe -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = C:\Users\Kade\Documents\westy\src\lib\glfw-3.3.2

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = C:\Users\Kade\Documents\westy\src\lib\glfw-3.3.2\build

# Include any dependencies generated for this target.
include examples/CMakeFiles/gears.dir/depend.make

# Include the progress variables for this target.
include examples/CMakeFiles/gears.dir/progress.make

# Include the compile flags for this target's objects.
include examples/CMakeFiles/gears.dir/flags.make

examples/CMakeFiles/gears.dir/gears.c.obj: examples/CMakeFiles/gears.dir/flags.make
examples/CMakeFiles/gears.dir/gears.c.obj: examples/CMakeFiles/gears.dir/includes_C.rsp
examples/CMakeFiles/gears.dir/gears.c.obj: ../examples/gears.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=C:\Users\Kade\Documents\westy\src\lib\glfw-3.3.2\build\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object examples/CMakeFiles/gears.dir/gears.c.obj"
	cd /d C:\Users\Kade\Documents\westy\src\lib\glfw-3.3.2\build\examples && C:\MinGW\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles\gears.dir\gears.c.obj -c C:\Users\Kade\Documents\westy\src\lib\glfw-3.3.2\examples\gears.c

examples/CMakeFiles/gears.dir/gears.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/gears.dir/gears.c.i"
	cd /d C:\Users\Kade\Documents\westy\src\lib\glfw-3.3.2\build\examples && C:\MinGW\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E C:\Users\Kade\Documents\westy\src\lib\glfw-3.3.2\examples\gears.c > CMakeFiles\gears.dir\gears.c.i

examples/CMakeFiles/gears.dir/gears.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/gears.dir/gears.c.s"
	cd /d C:\Users\Kade\Documents\westy\src\lib\glfw-3.3.2\build\examples && C:\MinGW\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S C:\Users\Kade\Documents\westy\src\lib\glfw-3.3.2\examples\gears.c -o CMakeFiles\gears.dir\gears.c.s

examples/CMakeFiles/gears.dir/glfw.rc.obj: examples/CMakeFiles/gears.dir/flags.make
examples/CMakeFiles/gears.dir/glfw.rc.obj: ../examples/glfw.rc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=C:\Users\Kade\Documents\westy\src\lib\glfw-3.3.2\build\CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building RC object examples/CMakeFiles/gears.dir/glfw.rc.obj"
	cd /d C:\Users\Kade\Documents\westy\src\lib\glfw-3.3.2\build\examples && C:\MinGW\bin\windres.exe -O coff $(RC_DEFINES) $(RC_INCLUDES) $(RC_FLAGS) C:\Users\Kade\Documents\westy\src\lib\glfw-3.3.2\examples\glfw.rc CMakeFiles\gears.dir\glfw.rc.obj

examples/CMakeFiles/gears.dir/__/deps/glad_gl.c.obj: examples/CMakeFiles/gears.dir/flags.make
examples/CMakeFiles/gears.dir/__/deps/glad_gl.c.obj: examples/CMakeFiles/gears.dir/includes_C.rsp
examples/CMakeFiles/gears.dir/__/deps/glad_gl.c.obj: ../deps/glad_gl.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=C:\Users\Kade\Documents\westy\src\lib\glfw-3.3.2\build\CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building C object examples/CMakeFiles/gears.dir/__/deps/glad_gl.c.obj"
	cd /d C:\Users\Kade\Documents\westy\src\lib\glfw-3.3.2\build\examples && C:\MinGW\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles\gears.dir\__\deps\glad_gl.c.obj -c C:\Users\Kade\Documents\westy\src\lib\glfw-3.3.2\deps\glad_gl.c

examples/CMakeFiles/gears.dir/__/deps/glad_gl.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/gears.dir/__/deps/glad_gl.c.i"
	cd /d C:\Users\Kade\Documents\westy\src\lib\glfw-3.3.2\build\examples && C:\MinGW\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E C:\Users\Kade\Documents\westy\src\lib\glfw-3.3.2\deps\glad_gl.c > CMakeFiles\gears.dir\__\deps\glad_gl.c.i

examples/CMakeFiles/gears.dir/__/deps/glad_gl.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/gears.dir/__/deps/glad_gl.c.s"
	cd /d C:\Users\Kade\Documents\westy\src\lib\glfw-3.3.2\build\examples && C:\MinGW\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S C:\Users\Kade\Documents\westy\src\lib\glfw-3.3.2\deps\glad_gl.c -o CMakeFiles\gears.dir\__\deps\glad_gl.c.s

# Object files for target gears
gears_OBJECTS = \
"CMakeFiles/gears.dir/gears.c.obj" \
"CMakeFiles/gears.dir/glfw.rc.obj" \
"CMakeFiles/gears.dir/__/deps/glad_gl.c.obj"

# External object files for target gears
gears_EXTERNAL_OBJECTS =

examples/gears.exe: examples/CMakeFiles/gears.dir/gears.c.obj
examples/gears.exe: examples/CMakeFiles/gears.dir/glfw.rc.obj
examples/gears.exe: examples/CMakeFiles/gears.dir/__/deps/glad_gl.c.obj
examples/gears.exe: examples/CMakeFiles/gears.dir/build.make
examples/gears.exe: src/libglfw3dll.a
examples/gears.exe: examples/CMakeFiles/gears.dir/linklibs.rsp
examples/gears.exe: examples/CMakeFiles/gears.dir/objects1.rsp
examples/gears.exe: examples/CMakeFiles/gears.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=C:\Users\Kade\Documents\westy\src\lib\glfw-3.3.2\build\CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Linking C executable gears.exe"
	cd /d C:\Users\Kade\Documents\westy\src\lib\glfw-3.3.2\build\examples && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\gears.dir\link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
examples/CMakeFiles/gears.dir/build: examples/gears.exe

.PHONY : examples/CMakeFiles/gears.dir/build

examples/CMakeFiles/gears.dir/clean:
	cd /d C:\Users\Kade\Documents\westy\src\lib\glfw-3.3.2\build\examples && $(CMAKE_COMMAND) -P CMakeFiles\gears.dir\cmake_clean.cmake
.PHONY : examples/CMakeFiles/gears.dir/clean

examples/CMakeFiles/gears.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" C:\Users\Kade\Documents\westy\src\lib\glfw-3.3.2 C:\Users\Kade\Documents\westy\src\lib\glfw-3.3.2\examples C:\Users\Kade\Documents\westy\src\lib\glfw-3.3.2\build C:\Users\Kade\Documents\westy\src\lib\glfw-3.3.2\build\examples C:\Users\Kade\Documents\westy\src\lib\glfw-3.3.2\build\examples\CMakeFiles\gears.dir\DependInfo.cmake --color=$(COLOR)
.PHONY : examples/CMakeFiles/gears.dir/depend

