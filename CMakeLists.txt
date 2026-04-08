cmake_minimum_required(VERSION 3.10)
project(any_ptr)

file(GLOB SOURCES "*.cpp")

if(SOURCES)
    add_executable(code ${SOURCES})
else()
    file(WRITE ${CMAKE_BINARY_DIR}/dummy.cpp "int main() { return 0; }")
    add_executable(code ${CMAKE_BINARY_DIR}/dummy.cpp)
endif()
