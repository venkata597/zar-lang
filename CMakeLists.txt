cmake_minimum_required(VERSION 3.10)

project(zar VERSION 0.1 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find LLVM using the config file
find_package(LLVM REQUIRED CONFIG)

# Print LLVM version (optional for debugging)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

# Include LLVM directories
include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

# Collect source files
file(GLOB SOURCES src/*.cpp)

# Define the executable
add_executable(zarc ${SOURCES})

# Add include directory for your project
target_include_directories(zarc PRIVATE include)

# Link LLVM libraries
llvm_map_components_to_libnames(llvm_libs core support irreader)

target_link_libraries(zarc PRIVATE ${llvm_libs})
