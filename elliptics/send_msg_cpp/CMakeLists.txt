cmake_minimum_required(VERSION 2.6)
project(elliptics_tests)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -W -Wall -Wextra -fstack-protector-all -fno-strict-aliasing -g -fPIC")
set(ECOMMON_LIBRARIES elliptics_client elliptics_cpp)
link_directories(/usr/lib)

find_package(Boost REQUIRED COMPONENTS iostreams thread regex program_options system filesystem)
message(STATUS "Boost information:")
message(STATUS "  Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
message(STATUS "  Boost_LIBRARY_DIRS: ${Boost_LIBRARY_DIRS}")
message(STATUS "  Boost_LIBRARIES: ${Boost_LIBRARIES}")
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

add_executable(elliptics_tests main.cpp)
target_link_libraries(elliptics_tests ${ECOMMON_LIBRARIES} ${Boost_LIBRARIES})
