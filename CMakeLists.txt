# CMakeList.txt : CMake project for spade, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.15...3.31)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy (SET CMP0141 NEW)
  set (CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif ()

# Project description and versioning.
project (spade
  DESCRIPTION "spade: A beautiful programming language"
  VERSION 1.0
  LANGUAGES CXX)

# Set C++ standard to C++20.
set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRED True)

# Set cmake module path
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# Set up boost
set (BOOST_ROOT "D:\\Programming\\Libraries\\boost_1_87_0")
set (Boost_USE_STATIC_LIBS ON)
set (Boost_USE_STATIC_RUNTIME OFF)

# Target: sputils
# It is the central utility library used by all other targets
file (GLOB_RECURSE SPUTILS_SRCS sputils/src/*.cpp)
file (GLOB_RECURSE SPUTILS_INCLUDES sputils/src/*.hpp)

add_library (sputils STATIC ${SPUTILS_SRCS} ${SPUTILS_INCLUDES})
target_include_directories (sputils PUBLIC sputils/src)

find_package (Boost 1.87.0 REQUIRED)
target_include_directories (sputils PUBLIC ${Boost_INCLUDE_DIRS})
target_link_libraries (sputils ${Boost_LIBRARIES})

# Target: velocity
# It is the implementation of the Spade Virtual Machine (VM) for Spade.
file (GLOB_RECURSE VELOCITY_SRCS velocity/src/*.cpp)
file (GLOB_RECURSE VELOCITY_INCLUDES velocity/src/*.hpp)

add_executable (velocity ${VELOCITY_SRCS} ${VELOCITY_INCLUDES})
target_include_directories (velocity PUBLIC velocity/src)
target_include_directories (velocity PUBLIC sputils/src)
target_link_libraries (velocity sputils)

# Target: spadec
# It is the compiler of the Spade language
file (GLOB_RECURSE SPADEC_SRCS spadec/src/*.cpp)
file (GLOB_RECURSE SPADEC_INCLUDES spadec/src/*.hpp)

add_executable (spadec ${SPADEC_SRCS} ${SPADEC_INCLUDES})
target_include_directories (spadec PUBLIC spadec/src)
target_include_directories (spadec PUBLIC sputils/src)
target_link_libraries (spadec sputils)

# TODO: Add tests and install targets if needed.
