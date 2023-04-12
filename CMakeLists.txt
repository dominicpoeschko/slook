cmake_minimum_required(VERSION 3.18)

project(slook)

add_subdirectory(aglio)

add_library(slook INTERFACE)
target_include_directories(slook INTERFACE src)
target_compile_features(slook INTERFACE cxx_std_20)
target_link_libraries(slook INTERFACE aglio)

add_library(slook::slook ALIAS slook)
