cmake_minimum_required(VERSION 3.13)
project(Mul2ShiftPass)

find_package(LLVM REQUIRED CONFIG)

add_library(Mul2ShiftPass MODULE Mul2Shift.cpp)

target_include_directories(Mul2ShiftPass PRIVATE ${LLVM_INCLUDE_DIRS})
target_link_libraries(Mul2ShiftPass PRIVATE LLVMCore LLVMSupport)

set_target_properties(Mul2ShiftPass PROPERTIES
    CXX_STANDARD 17
    POSITION_INDEPENDENT_CODE ON
)
