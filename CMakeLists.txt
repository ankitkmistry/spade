# CMakeList.txt : CMake project for spade, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.15...3.31)

# Project description and versioning.
project (spade
    DESCRIPTION "spade: A beautiful programming language"
    VERSION 1.0)

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set C++ standard to C++20.
set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRED True)

# Set cmake module path
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set (LLVM_DIR thirdparty/llvm/lib/cmake/llvm)

# Set up llvm
find_package (LLVM REQUIRED CONFIG)

# Set up boost
set (BOOST_ROOT "D:\\Programming\\Libraries\\boost_1_87_0")
set (Boost_USE_STATIC_LIBS ON)
set (Boost_USE_STATIC_RUNTIME OFF)

# Set up cpptrace
include (FetchContent)
FetchContent_Declare (
    cpptrace
    GIT_REPOSITORY https://github.com/jeremy-rifkin/cpptrace.git
    GIT_TAG v0.8.3 # <HASH or TAG>
)
FetchContent_MakeAvailable (cpptrace)

# Set up argparse
include (FetchContent)
FetchContent_Declare (
    argparse
    GIT_REPOSITORY https://github.com/p-ranav/argparse.git
)
FetchContent_MakeAvailable (argparse)

# Set up spdlog
find_package(spdlog REQUIRED CONFIG)

# Target: sputils
# It is the central utility library used by all other targets
file (GLOB_RECURSE SPUTILS_SRCS sputils/src/*.cpp)
file (GLOB_RECURSE SPUTILS_INCLUDES sputils/src/*.hpp)

add_library (sputils STATIC ${SPUTILS_SRCS} ${SPUTILS_INCLUDES})

target_include_directories (sputils PUBLIC sputils/src)

find_package (Boost 1.74.0 REQUIRED)
target_include_directories (sputils PUBLIC ${Boost_INCLUDE_DIRS})
target_link_libraries (sputils ${Boost_LIBRARIES})

# Target: spadec
# It is the compiler of the Spade language
file (GLOB_RECURSE SPADEC_SRCS spadec/src/*.cpp)
file (GLOB_RECURSE SPADEC_INCLUDES spadec/src/*.hpp)

add_executable (spadec ${SPADEC_SRCS} ${SPADEC_INCLUDES})
target_include_directories (spadec PRIVATE spadec/src)
target_include_directories (spadec PUBLIC sputils/src)
target_link_libraries (spadec PUBLIC sputils PRIVATE cpptrace::cpptrace spdlog::spdlog)

# Target: spasm
# It is the assembler of the Spade language bytecode
file (GLOB_RECURSE SPASM_SRCS spasm/src/*.cpp)
file (GLOB_RECURSE SPASM_INCLUDES spasm/src/*.hpp)

add_executable (spasm ${SPASM_SRCS} ${SPASM_INCLUDES})
target_include_directories (spasm PRIVATE spasm/src)
target_include_directories (spasm PUBLIC sputils/src)
target_link_libraries (spasm PUBLIC sputils PRIVATE argparse)

# Target: velocity
# It is the implementation of the Spade Virtual Machine (VM) for Spade.
file (GLOB_RECURSE VELOCITY_SRCS velocity/src/*.cpp)
file (GLOB_RECURSE VELOCITY_INCLUDES velocity/src/*.hpp)

add_executable (velocity ${VELOCITY_SRCS} ${VELOCITY_INCLUDES})

# Reference: https://llvm.org/docs/CMake.html
target_include_directories (velocity PUBLIC velocity/src PUBLIC sputils/src PRIVATE ${LLVM_INCLUDE_DIRS})
separate_arguments (LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions (${LLVM_DEFINITIONS_LIST})

# Find the libraries that correspond to the LLVM components
llvm_map_components_to_libnames (LLVM_LIBS core support orcjit native)
target_link_libraries (velocity PUBLIC sputils PRIVATE ${LLVM_LIBS})

# Target: swan
# It is the new implementation of the Spade Virtual Machine (VM) for Spade.
file (GLOB_RECURSE SWAN_SRCS swan/src/*.cpp)
file (GLOB_RECURSE SWAN_INCLUDES swan/src/*.hpp)

add_executable (swan ${SWAN_SRCS} ${SWAN_INCLUDES})

# Reference: https://llvm.org/docs/CMake.html
target_include_directories (swan PUBLIC swan/src PUBLIC sputils/src PRIVATE ${LLVM_INCLUDE_DIRS})
separate_arguments (LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions (${LLVM_DEFINITIONS_LIST})

# Find the libraries that correspond to the LLVM components
llvm_map_components_to_libnames (LLVM_LIBS core support orcjit native)
target_link_libraries (swan PUBLIC sputils PRIVATE ${LLVM_LIBS})

if (MSVC)
    # warning level 4
    # target_compile_options (sputils PRIVATE /W4)
    # target_compile_options (spadec PRIVATE /W4)
    # target_compile_options (spasm PRIVATE /W4)
    # target_compile_options (velocity PRIVATE /W4)
    target_compile_options (sputils PRIVATE /MD)
    target_compile_definitions (sputils PRIVATE _ITERATOR_DEBUG_LEVEL=0)

    target_compile_options (spadec PRIVATE /MD)
    target_compile_definitions (spadec PRIVATE _ITERATOR_DEBUG_LEVEL=0)

    target_compile_options (spasm PRIVATE /MD)
    target_compile_definitions (spasm PRIVATE _ITERATOR_DEBUG_LEVEL=0)

    target_compile_options (velocity PRIVATE /MD)
    target_compile_definitions (velocity PRIVATE _ITERATOR_DEBUG_LEVEL=0)
else ()
    # additional warnings
    target_compile_options (sputils PRIVATE -Wall -Wextra -Wno-unused-parameter)
    target_compile_options (spadec PRIVATE -Wall -Wextra -Wno-unused-parameter)
    target_compile_options (spasm PRIVATE -Wall -Wextra)
    target_compile_options (velocity PRIVATE -Wall -Wextra)
endif ()

if (WIN32)
    add_custom_command (
        TARGET spadec POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:cpptrace::cpptrace>
        $<TARGET_FILE_DIR:spadec>
    )
endif ()

if (APPLE)
    add_custom_command (
        TARGET spadec
        POST_BUILD
        COMMAND dsymutil $<TARGET_FILE:spadec>
    )
endif ()

if (CMAKE_BUILD_TYPE EQUAL "RelWithDebInfo")
    if (MSVC)
        target_compile_options (spadec /EHsc)
    endif ()
endif ()

# TODO: Add tests and install targets if needed.
