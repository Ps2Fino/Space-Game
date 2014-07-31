# This is the global CMakeLists.txt file for the project
# @author Daniel J. Finnegan
# @date July 2014

cmake_minimum_required (VERSION 3.0)

# The project name
project (ROCKET_SHOOTER)

# Add the cmake module for finding SDL 2
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${ROCKET_SHOOTER_SOURCE_DIR}/cmake")

########################################################
####### Main Executable build commands #################
########################################################

# Grab the sources for the main executable
set (ROCKET_SHOOTER_SOURCE)
file (GLOB ROCKET_SHOOTER_SRC
	${ROCKET_SHOOTER_SOURCE_DIR}/src/*.cpp
)
list (APPEND ROCKET_SHOOTER_SOURCE ${ROCKET_SHOOTER_SRC})

# Include the headers
include_directories (${ROCKET_SHOOTER_SOURCE_DIR}/include)

# Specify the project executable
add_executable (rocket_shooter ${ROCKET_SHOOTER_SOURCE})

########################################################
####### Link command for the SDL library ###############
########################################################

# Create a variable for housing the required libraries for the main executable
set (ROCKET_SHOOTER_REQUIRED_LIBS)

# Search for SDL 2 and append the library to the required libs variable
find_package (SDL2 REQUIRED)
include_directories (${SDL2_INCLUDE_DIR})
list (APPEND ROCKET_SHOOTER_REQUIRED_LIBS ${SDL2_LIBRARY})

# Search for SDL Image and append it to the list
find_package (SDL2_image REQUIRED)
include_directories (${SDL2_IMAGE_INCLUDE_DIR})
list (APPEND ROCKET_SHOOTER_REQUIRED_LIBS ${SDL2_IMAGE_LIBRARY})

# Tell cmake to link against the SDL library
target_link_libraries (rocket_shooter ${ROCKET_SHOOTER_REQUIRED_LIBS})

########################################################
####### Installation rules for the executable ##########
########################################################

# Set the directory for the binary to be placed and add the install rule to the generated make file
set (BIN_DIR ${ROCKET_SHOOTER_SOURCE_DIR}/bin)
install (TARGETS rocket_shooter RUNTIME DESTINATION ${BIN_DIR})