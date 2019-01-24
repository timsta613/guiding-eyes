# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.5

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
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
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/tim/guiding-eyes/algo

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/tim/guiding-eyes/algo/build

# Include any dependencies generated for this target.
include CMakeFiles/create-dataset.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/create-dataset.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/create-dataset.dir/flags.make

CMakeFiles/create-dataset.dir/create-dataset.cpp.o: CMakeFiles/create-dataset.dir/flags.make
CMakeFiles/create-dataset.dir/create-dataset.cpp.o: ../create-dataset.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/tim/guiding-eyes/algo/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/create-dataset.dir/create-dataset.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/create-dataset.dir/create-dataset.cpp.o -c /home/tim/guiding-eyes/algo/create-dataset.cpp

CMakeFiles/create-dataset.dir/create-dataset.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/create-dataset.dir/create-dataset.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/tim/guiding-eyes/algo/create-dataset.cpp > CMakeFiles/create-dataset.dir/create-dataset.cpp.i

CMakeFiles/create-dataset.dir/create-dataset.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/create-dataset.dir/create-dataset.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/tim/guiding-eyes/algo/create-dataset.cpp -o CMakeFiles/create-dataset.dir/create-dataset.cpp.s

CMakeFiles/create-dataset.dir/create-dataset.cpp.o.requires:

.PHONY : CMakeFiles/create-dataset.dir/create-dataset.cpp.o.requires

CMakeFiles/create-dataset.dir/create-dataset.cpp.o.provides: CMakeFiles/create-dataset.dir/create-dataset.cpp.o.requires
	$(MAKE) -f CMakeFiles/create-dataset.dir/build.make CMakeFiles/create-dataset.dir/create-dataset.cpp.o.provides.build
.PHONY : CMakeFiles/create-dataset.dir/create-dataset.cpp.o.provides

CMakeFiles/create-dataset.dir/create-dataset.cpp.o.provides.build: CMakeFiles/create-dataset.dir/create-dataset.cpp.o


# Object files for target create-dataset
create__dataset_OBJECTS = \
"CMakeFiles/create-dataset.dir/create-dataset.cpp.o"

# External object files for target create-dataset
create__dataset_EXTERNAL_OBJECTS =

create-dataset: CMakeFiles/create-dataset.dir/create-dataset.cpp.o
create-dataset: CMakeFiles/create-dataset.dir/build.make
create-dataset: /usr/local/lib/librealsense2.so
create-dataset: /usr/local/lib/libwiringPi.so
create-dataset: /usr/lib/arm-linux-gnueabihf/libcrypt.so
create-dataset: /usr/local/lib/libopencv_ml.so.4.0.0
create-dataset: /usr/local/lib/libopencv_videostab.so.4.0.0
create-dataset: /usr/local/lib/libopencv_superres.so.4.0.0
create-dataset: /usr/local/lib/libopencv_photo.so.4.0.0
create-dataset: /usr/local/lib/libopencv_stitching.so.4.0.0
create-dataset: /usr/local/lib/libopencv_dnn.so.4.0.0
create-dataset: /usr/local/lib/libopencv_gapi.so.4.0.0
create-dataset: /usr/local/lib/libopencv_shape.so.4.0.0
create-dataset: /usr/local/lib/libopencv_objdetect.so.4.0.0
create-dataset: /usr/local/lib/libopencv_video.so.4.0.0
create-dataset: /usr/local/lib/libopencv_calib3d.so.4.0.0
create-dataset: /usr/local/lib/libopencv_features2d.so.4.0.0
create-dataset: /usr/local/lib/libopencv_highgui.so.4.0.0
create-dataset: /usr/local/lib/libopencv_videoio.so.4.0.0
create-dataset: /usr/local/lib/libopencv_imgcodecs.so.4.0.0
create-dataset: /usr/local/lib/libopencv_imgproc.so.4.0.0
create-dataset: /usr/local/lib/libopencv_flann.so.4.0.0
create-dataset: /usr/local/lib/libopencv_core.so.4.0.0
create-dataset: CMakeFiles/create-dataset.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/tim/guiding-eyes/algo/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable create-dataset"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/create-dataset.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/create-dataset.dir/build: create-dataset

.PHONY : CMakeFiles/create-dataset.dir/build

CMakeFiles/create-dataset.dir/requires: CMakeFiles/create-dataset.dir/create-dataset.cpp.o.requires

.PHONY : CMakeFiles/create-dataset.dir/requires

CMakeFiles/create-dataset.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/create-dataset.dir/cmake_clean.cmake
.PHONY : CMakeFiles/create-dataset.dir/clean

CMakeFiles/create-dataset.dir/depend:
	cd /home/tim/guiding-eyes/algo/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/tim/guiding-eyes/algo /home/tim/guiding-eyes/algo /home/tim/guiding-eyes/algo/build /home/tim/guiding-eyes/algo/build /home/tim/guiding-eyes/algo/build/CMakeFiles/create-dataset.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/create-dataset.dir/depend
