cmake_minimum_required(VERSION 3.20)
project(grayscott VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

find_package(MPI REQUIRED COMPONENTS CXX)

add_executable(grayscott src/grayscott.cpp)
target_link_libraries(grayscott PRIVATE MPI::MPI_CXX)
target_compile_definitions(grayscott PRIVATE GRAYSCOTT_VERSION="${PROJECT_VERSION}")

include(GNUInstallDirs)
install(TARGETS grayscott RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
