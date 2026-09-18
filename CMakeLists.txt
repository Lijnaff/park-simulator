cmake_minimum_required(VERSION 3.10)
project(ParkSimulator VERSION 1.0)

# Specify C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add header search paths
include_directories(include)

# Find OpenGL and GLUT on macOS
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

# Collect source files
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    ${OPENGL_LIBRARIES}
    ${GLUT_LIBRARIES}
)

# Optional: Add compiler flags (e.g., warnings)
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wno-deprecated-declarations)
endif()
