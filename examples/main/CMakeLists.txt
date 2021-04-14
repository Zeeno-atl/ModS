cmake_minimum_required(VERSION 3.5)

project(example LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(base_app main.cpp)

target_include_directories(base_app PUBLIC ../../include)
target_link_libraries(base_app mods)
if(UNIX)
    target_link_libraries(base_app dl)
endif(UNIX)

set(Boost_USE_STATIC_LIBS        ON)  # only find static libs
set(Boost_USE_DEBUG_LIBS         OFF) # ignore debug libs and
set(Boost_USE_RELEASE_LIBS       ON)  # only find release libs
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)
find_library(BOOST 1.70 COMPONENTS filesystem system)

