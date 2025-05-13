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

# Set up cpptrace
include (FetchContent)
FetchContent_Declare (
  cpptrace
  GIT_REPOSITORY https://github.com/jeremy-rifkin/cpptrace.git
  GIT_TAG v0.8.3 # <HASH or TAG>
)
FetchContent_MakeAvailable (cpptrace)

# Target: sputils
# It is the central utility library used by all other targets
file (GLOB_RECURSE SPUTILS_SRCS sputils/src/*.cpp)
file (GLOB_RECURSE SPUTILS_INCLUDES sputils/src/*.hpp)

add_library (sputils STATIC ${SPUTILS_SRCS} ${SPUTILS_INCLUDES})
target_include_directories (sputils PUBLIC sputils/src)

find_package (Boost 1.74.0 REQUIRED)
target_include_directories (sputils PUBLIC ${Boost_INCLUDE_DIRS})
target_link_libraries (sputils ${Boost_LIBRARIES})

# Target: velocity
# It is the implementation of the Spade Virtual Machine (VM) for Spade.
file (GLOB_RECURSE VELOCITY_SRCS velocity/src/*.cpp)
file (GLOB_RECURSE VELOCITY_INCLUDES velocity/src/*.hpp)

add_executable (velocity ${VELOCITY_SRCS} ${VELOCITY_INCLUDES})
target_include_directories (velocity PRIVATE velocity/src)
target_include_directories (velocity PUBLIC sputils/src)
target_link_libraries (velocity sputils)

# Target: spadec
# It is the compiler of the Spade language
file (GLOB_RECURSE SPADEC_SRCS spadec/src/*.cpp)
file (GLOB_RECURSE SPADEC_INCLUDES spadec/src/*.hpp)

add_executable (spadec ${SPADEC_SRCS} ${SPADEC_INCLUDES})
target_include_directories (spadec PRIVATE spadec/src)
target_include_directories (spadec PUBLIC sputils/src)
target_link_libraries (spadec PUBLIC sputils PRIVATE cpptrace::cpptrace)

# Target: spasm
# It is the assembler of the Spade language bytecode
file (GLOB_RECURSE SPASM_SRCS spasm/src/*.cpp)
file (GLOB_RECURSE SPASM_INCLUDES spasm/src/*.hpp)

add_executable (spasm ${SPASM_SRCS} ${SPASM_INCLUDES})
target_include_directories (spasm PRIVATE spasm/src)
target_include_directories (spasm PUBLIC sputils/src)
target_link_libraries (spasm PUBLIC sputils)

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

if(CMAKE_BUILD_TYPE EQUAL "RelWithDebInfo")
  if(MSVC)
    target_compile_options(spadec /EHsc)
  endif()
endif()

# TODO: Add tests and install targets if needed.
