cmake_minimum_required(VERSION 3.27)
project(crafting_interpreter)

set(CMAKE_CXX_STANDARD 17)
add_compile_options("-fno-rtti")

find_package(LLVM REQUIRED CONFIG)
link_directories(${LLVM_BUILD_LIBRARY_DIR})
link_libraries(${LLVM_AVAILABLE_LIBS})

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_BINARY_DIR}/include)

add_subdirectory(lib)
add_subdirectory(test)



add_executable(crafting_interpreter main.cpp)

add_subdirectory(third-party/doctest)
