cmake_minimum_required(VERSION 3.7)
project(Mila)

# LLVM
find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

llvm_map_components_to_libnames(llvm_libs support core irreader
        x86 x86asmparser x86asmprinter x86codegen x86desc x86disassembler x86info x86utils )

set(SOURCE_FILES main.cpp)

add_executable(Mila ${SOURCE_FILES})
target_link_libraries(Mila ${llvm_libs})
