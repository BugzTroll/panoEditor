cmake_minimum_required(VERSION 3.1.0)

project(panoramaeditor)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)
# Create code from a list of Qt designer ui files
set(CMAKE_AUTOUIC ON)

# Find the QtWidgets library
find_package(Qt5Widgets CONFIG REQUIRED)

# find include
include_directories(src)

# Populate a CMake variable with the sources
file(GLOB panoeditor_SRCS
    "src/*.h"
    "src/*.cpp"
    "src/*.ui"
    "src/shaders/*"
)

# Tell CMake to create the panoeditor executable
add_executable(panoeditor ${panoeditor_SRCS})
# Use the Widgets module from Qt 5
target_link_libraries(panoeditor Qt5::Widgets Qt5::Gui)
