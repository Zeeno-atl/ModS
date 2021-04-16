cmake_minimum_required(VERSION 3.5)

project(ModS LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#set(CMAKE_CXX_CLANG_TIDY "clang-tidy")

add_subdirectory(examples)

install(FILES include/ModS/Module.hpp include/ModS/AbstractFactory.hpp include/ModS/Factory.hpp DESTINATION include/ModS)
install(FILES include/Signal/Signal.hpp DESTINATION include/Signal)

add_library(mods STATIC src/Injector.cpp)
target_include_directories(mods PUBLIC include)

find_library(BOOST 1.75 REQUIRED)
target_include_directories(mods PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(mods boost_system boost_filesystem)

install(TARGETS mods DESTINATION lib/)
