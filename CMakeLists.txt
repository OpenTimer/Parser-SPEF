cmake_minimum_required (VERSION 3.9)
MESSAGE(STATUS "CMAKE_ROOT: " ${CMAKE_ROOT})

# Project name
project(Parser-SPEF)

include(CTest)

# Turn on the verbose
set(CMAKE_VERBOSE_MAKEFILE ON)

# CXX target properties
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
#set(USE_WERROR ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O2")
message(STATUS "CMAKE_BUILD_TYPE: " ${CMAKE_BUILD_TYPE})
message(STATUS "CMAKE_CXX_COMPILER: " ${CMAKE_CXX_COMPILER})
message(STATUS "CMAKE_CXX_FLAGS: " ${CMAKE_CXX_FLAGS})
message(STATUS "CMAKE_EXE_LINKER_FLAGS: " ${CMAKE_EXE_LINKER_FLAGS})


# add the binary tree to the search path for include files
add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE parser-spef)

# -----------------------------------------------------------------------------
# Example program
# -----------------------------------------------------------------------------
message(STATUS "Building examples ...")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/example)

message(STATUS "EXAMPLE_CXX_FLAGS: " ${EXAMPLE_CXX_FLAGS})
message(STATUS "EXAMPLE_EXE_LINKER_FLAGS: " ${EXAMPLE_EXE_LINKER_FLAGS})

add_executable(parser-spef-example example/simple.cpp)
target_link_libraries(parser-spef-example stdc++fs ${PROJECT_NAME})

# -----------------------------------------------------------------------------
# Unittest
# -----------------------------------------------------------------------------
enable_testing()

message(STATUS "Building unit tests ...")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/unittest)

add_executable(parser-spef-test unittest/parser-spef.cpp)
target_link_libraries(parser-spef-test stdc++fs ${PROJECT_NAME})
target_include_directories(parser-spef-test PRIVATE doctest)

add_executable(benchmark unittest/benchmark.cpp)
target_link_libraries(benchmark stdc++fs ${PROJECT_NAME})
target_include_directories(benchmark PRIVATE doctest)


add_test(HeaderFix ${PROJECT_SOURCE_DIR}/unittest/parser-spef -tc=Header.Fix)
add_test(HeaderRandom ${PROJECT_SOURCE_DIR}/unittest/parser-spef -tc=Header.Random)
add_test(NameMap ${PROJECT_SOURCE_DIR}/unittest/parser-spef -tc=NameMap)
add_test(Port ${PROJECT_SOURCE_DIR}/unittest/parser-spef -tc=Port)
add_test(Net ${PROJECT_SOURCE_DIR}/unittest/parser-spef -tc=Net)
add_test(NameExpansion ${PROJECT_SOURCE_DIR}/unittest/parser-spef -tc=NAME_EXPANSION)
add_test(Benchmark ${PROJECT_SOURCE_DIR}/unittest/benchmark)



# INSTALL
# Installation target
install(
  TARGETS Parser-SPEF
  EXPORT Parser-SPEFTargets
  ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_PREFIX}/include/
)

# Export targets to install path
# install(
#   EXPORT Parser-SPEFTargets
#   FILE Parser-SPEFTargets.cmake
#   DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/cmake/Parser-SPEFTargets
# )

# Support use directly in build tree
export(EXPORT Parser-SPEFTargets
  FILE "${CMAKE_CURRENT_BINARY_DIR}/cmake/Parser-SPEFTargets.cmake"
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/Parser-SPEFConfigVersion.cmake
    VERSION 3.9
    COMPATIBILITY ExactVersion
    ARCH_INDEPENDENT
)

configure_package_config_file( Parser-SPEFConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/Parser-SPEFConfig.cmake
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Parser-SPEF
)

export(EXPORT Parser-SPEFTargets
  FILE "${CMAKE_CURRENT_BINARY_DIR}/cmake/Parser-SPEF.cmake"
  NAMESPACE Parser-SPEF::
)

install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/Parser-SPEFConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/Parser-SPEFConfigVersion.cmake
    DESTINATION
        ${CMAKE_INSTALL_LIBDIR}/cmake/Parser-SPEF
)

