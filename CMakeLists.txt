#Conan version

cmake_minimum_required(VERSION 3.5)
project("ModS" LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#set(CMAKE_CXX_CLANG_TIDY "clang-tidy")

set(MODS_DIR "${CMAKE_SOURCE_DIR}/submodules/ModS")

file(GLOB_RECURSE sourceFiles
    "${MODS_DIR}/src/*.cpp"
    "${MODS_DIR}/include/*.hpp"
)

add_library(${PROJECT_NAME}_injector STATIC "src/Injector.cpp")
add_library(${PROJECT_NAME}_module INTERFACE)
set_target_properties(${PROJECT_NAME}_injector PROPERTIES FOLDER "submodule/3p")
set_target_properties(${PROJECT_NAME}_module PROPERTIES FOLDER "submodule/3p")

target_include_directories(${PROJECT_NAME}_injector PUBLIC "${MODS_DIR}/include")
target_include_directories(${PROJECT_NAME}_module INTERFACE "${MODS_DIR}/include")

find_package(Boost REQUIRED CONFIG)
target_link_libraries(${PROJECT_NAME}_injector PRIVATE Boost::Boost PUBLIC Signal)
target_link_libraries(${PROJECT_NAME}_module INTERFACE Signal)
