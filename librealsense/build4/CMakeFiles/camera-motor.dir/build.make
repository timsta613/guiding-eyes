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
CMAKE_SOURCE_DIR = /home/tim/guiding-eyes/librealsense

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/tim/guiding-eyes/librealsense/build4

# Include any dependencies generated for this target.
include CMakeFiles/camera-motor.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/camera-motor.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/camera-motor.dir/flags.make

CMakeFiles/camera-motor.dir/camera-motor.cpp.o: CMakeFiles/camera-motor.dir/flags.make
CMakeFiles/camera-motor.dir/camera-motor.cpp.o: ../camera-motor.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/tim/guiding-eyes/librealsense/build4/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/camera-motor.dir/camera-motor.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/camera-motor.dir/camera-motor.cpp.o -c /home/tim/guiding-eyes/librealsense/camera-motor.cpp

CMakeFiles/camera-motor.dir/camera-motor.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/camera-motor.dir/camera-motor.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/tim/guiding-eyes/librealsense/camera-motor.cpp > CMakeFiles/camera-motor.dir/camera-motor.cpp.i

CMakeFiles/camera-motor.dir/camera-motor.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/camera-motor.dir/camera-motor.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/tim/guiding-eyes/librealsense/camera-motor.cpp -o CMakeFiles/camera-motor.dir/camera-motor.cpp.s

CMakeFiles/camera-motor.dir/camera-motor.cpp.o.requires:

.PHONY : CMakeFiles/camera-motor.dir/camera-motor.cpp.o.requires

CMakeFiles/camera-motor.dir/camera-motor.cpp.o.provides: CMakeFiles/camera-motor.dir/camera-motor.cpp.o.requires
	$(MAKE) -f CMakeFiles/camera-motor.dir/build.make CMakeFiles/camera-motor.dir/camera-motor.cpp.o.provides.build
.PHONY : CMakeFiles/camera-motor.dir/camera-motor.cpp.o.provides

CMakeFiles/camera-motor.dir/camera-motor.cpp.o.provides.build: CMakeFiles/camera-motor.dir/camera-motor.cpp.o


# Object files for target camera-motor
camera__motor_OBJECTS = \
"CMakeFiles/camera-motor.dir/camera-motor.cpp.o"

# External object files for target camera-motor
camera__motor_EXTERNAL_OBJECTS =

camera-motor: CMakeFiles/camera-motor.dir/camera-motor.cpp.o
camera-motor: CMakeFiles/camera-motor.dir/build.make
camera-motor: /usr/local/lib/librealsense2.so
camera-motor: /usr/local/lib/libwiringPi.so
camera-motor: /usr/lib/arm-linux-gnueabihf/libcrypt.so
camera-motor: /usr/local/lib/libopencv_ml.so.4.0.0
camera-motor: /usr/local/lib/libopencv_videostab.so.4.0.0
camera-motor: /usr/local/lib/libopencv_superres.so.4.0.0
camera-motor: /usr/local/lib/libopencv_photo.so.4.0.0
camera-motor: /usr/local/lib/libopencv_stitching.so.4.0.0
camera-motor: /usr/local/lib/libopencv_dnn.so.4.0.0
camera-motor: /usr/local/lib/libopencv_gapi.so.4.0.0
camera-motor: /usr/local/lib/libopencv_shape.so.4.0.0
camera-motor: /usr/local/lib/libopencv_objdetect.so.4.0.0
camera-motor: /usr/local/lib/libopencv_video.so.4.0.0
camera-motor: /usr/local/lib/libopencv_calib3d.so.4.0.0
camera-motor: /usr/local/lib/libopencv_features2d.so.4.0.0
camera-motor: /usr/local/lib/libopencv_highgui.so.4.0.0
camera-motor: /usr/local/lib/libopencv_videoio.so.4.0.0
camera-motor: /usr/local/lib/libopencv_imgcodecs.so.4.0.0
camera-motor: /usr/local/lib/libopencv_imgproc.so.4.0.0
camera-motor: /usr/local/lib/libopencv_flann.so.4.0.0
camera-motor: /usr/local/lib/libopencv_core.so.4.0.0
camera-motor: CMakeFiles/camera-motor.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/tim/guiding-eyes/librealsense/build4/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable camera-motor"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/camera-motor.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/camera-motor.dir/build: camera-motor

.PHONY : CMakeFiles/camera-motor.dir/build

CMakeFiles/camera-motor.dir/requires: CMakeFiles/camera-motor.dir/camera-motor.cpp.o.requires

.PHONY : CMakeFiles/camera-motor.dir/requires

CMakeFiles/camera-motor.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/camera-motor.dir/cmake_clean.cmake
.PHONY : CMakeFiles/camera-motor.dir/clean

CMakeFiles/camera-motor.dir/depend:
	cd /home/tim/guiding-eyes/librealsense/build4 && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/tim/guiding-eyes/librealsense /home/tim/guiding-eyes/librealsense /home/tim/guiding-eyes/librealsense/build4 /home/tim/guiding-eyes/librealsense/build4 /home/tim/guiding-eyes/librealsense/build4/CMakeFiles/camera-motor.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/camera-motor.dir/depend

