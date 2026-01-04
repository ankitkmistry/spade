# CMakeList.txt : CMake project for spade, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.19)

# Project description and versioning.
project (spade
    DESCRIPTION "spade: A beautiful programming language"
    VERSION 1.0)

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set C++ standard to C++20.
set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

# Set cmake module path
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set (LLVM_DIR thirdparty/llvm/lib/cmake/llvm)

# Set up boost
set (Boost_USE_STATIC_LIBS ON)
set (Boost_USE_STATIC_RUNTIME OFF)

# find_package(cpptrace REQUIRED CONFIG)
find_package(argparse REQUIRED CONFIG)
find_package(spdlog REQUIRED CONFIG)
find_package(libffi REQUIRED CONFIG)
find_package(Boost REQUIRED COMPONENTS core predef container_hash)

# Set up git submodules
find_package(Git QUIET)
if (EXISTS "${PROJECT_SOURCE_DIR}/.git")
    option(GIT_SUBMODULE "Check submodules during build" ON)
    if (GIT_SUBMODULE) 
        message(STATUS "Submodule update")
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive --force --rebase
                        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                        RESULT_VARIABLE GIT_SUBMOD_RESULT)
        if (NOT GIT_SUBMOD_RESULT EQUAL "0") 
            message(FATAL_ERROR "git submodule update --init --recursive --force failed with exit code ${GIT_SUBMOD_RESULT}")
        endif ()
    endif ()
endif ()

add_subdirectory(external/nite)

# Target: sputils
# It is the central utility library used by all other targets
file (GLOB_RECURSE SPUTILS_SRCS sputils/src/*.cpp)
file (GLOB_RECURSE SPUTILS_INCLUDES sputils/src/*.hpp)

add_library (sputils STATIC ${SPUTILS_SRCS})

target_include_directories (sputils PUBLIC sputils/src)
target_include_directories (sputils PUBLIC ${Boost_INCLUDE_DIRS})
target_link_libraries (sputils ${Boost_LIBRARIES})

# Target: spadec
# It is the compiler of the Spade language
file (GLOB_RECURSE SPADEC_SRCS spadec/src/*.cpp)
file (GLOB_RECURSE SPADEC_INCLUDES spadec/src/*.hpp)

add_executable (spadec ${SPADEC_SRCS})
target_include_directories (spadec PUBLIC spadec/src)
target_include_directories (spadec PUBLIC sputils/src)
# target_link_libraries (spadec PUBLIC sputils PRIVATE cpptrace::cpptrace spdlog::spdlog)
target_link_libraries (spadec PUBLIC sputils PRIVATE spdlog::spdlog)

# Target: spasm
# It is the assembler of the Spade language bytecode
file (GLOB_RECURSE SPASM_SRCS spasm/src/*.cpp)
file (GLOB_RECURSE SPASM_INCLUDES spasm/src/*.hpp)

add_executable (spasm ${SPASM_SRCS})
target_include_directories (spasm PUBLIC spasm/src)
target_include_directories (spasm PUBLIC sputils/src)
target_link_libraries (spasm PUBLIC sputils PRIVATE argparse::argparse)

# Target: swan
# It is the new implementation of the Spade Virtual Machine (VM) for Spade.
file (GLOB_RECURSE SWAN_SRCS swan/src/*.cpp)
file (GLOB_RECURSE SWAN_INCLUDES swan/src/*.hpp)

add_executable (swan ${SWAN_SRCS})
target_include_directories (swan PUBLIC swan/src)
target_include_directories (swan PUBLIC sputils/src)

# # Set up llvm
# # Reference: https://llvm.org/docs/CMake.html
# find_package (LLVM REQUIRED CONFIG)
# target_include_directories (swan PUBLIC swan/src PUBLIC sputils/src PRIVATE ${LLVM_INCLUDE_DIRS})
# separate_arguments (LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
# add_definitions (${LLVM_DEFINITIONS_LIST})
#
# # Find the libraries that correspond to the LLVM components
# llvm_map_components_to_libnames (LLVM_LIBS core support orcjit native)
# target_link_libraries (swan PUBLIC sputils PRIVATE spdlog::spdlog ${LLVM_LIBS})

target_link_libraries (swan PUBLIC sputils PRIVATE nite::nite spdlog::spdlog)

if (MSVC)
    # warning level 4
    # target_compile_options (sputils PRIVATE /W4)
    # target_compile_options (spadec PRIVATE /W4)
    # target_compile_options (spasm PRIVATE /W4)
    target_compile_options (sputils PRIVATE /MD)
    target_compile_definitions (sputils PRIVATE _ITERATOR_DEBUG_LEVEL=0)

    target_compile_options (spadec PRIVATE /MD)
    target_compile_definitions (spadec PRIVATE _ITERATOR_DEBUG_LEVEL=0)

    target_compile_options (spasm PRIVATE /MD)
    target_compile_definitions (spasm PRIVATE _ITERATOR_DEBUG_LEVEL=0)
else ()
    # additional warnings
    target_compile_options (sputils PRIVATE -Wall -Wextra)
    target_compile_options (spadec PRIVATE -Wall -Wextra)
    target_compile_options (spasm PRIVATE -Wall -Wextra)
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
