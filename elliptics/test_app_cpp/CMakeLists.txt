cmake_minimum_required(VERSION 2.6)
project(test_app_cpp)

set(CMAKE_CXX_FLAGS "-std=c++11 -W -Wall -Wextra -fstack-protector-all -fno-strict-aliasing -g -fPIC")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

find_package(Boost REQUIRED COMPONENTS thread system)
find_package(CocaineNative REQUIRED)
find_package(elliptics REQUIRED)

include(cmake/Modules/locate_library.cmake)
LOCATE_LIBRARY(LIBEV "ev++.h" "ev" "libev")

include_directories(${elliptics_INCLUDE_DIRS} ${CocaineNative_INCLUDE_DIRS} ${LIBEV_INCLUDE_DIRS})


add_executable(test_app_cpp main.cpp)
target_link_libraries(srw_app_cpp elliptics_cpp ${CocaineNative_LIBRARIES} ${Boost_LIBRARIES}  ${elliptics_cpp_LIBRARIES} ${elliptics_LIBRARIES} ${LIBEV_LIBRARIES})

