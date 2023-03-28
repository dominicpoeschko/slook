cmake_minimum_required(VERSION 3.18)

project(slook)

add_subdirectory(aglio)
add_subdirectory(recreated_goon)

recreated_goon_generate(
  slook_type_descriptors OUTPUT_DIRECTORY slook_type_descriptors FILES
  src/slook/types.hpp src/slook/commands.hpp)


  include(FetchContent)
    FetchContent_Declare(
      fmt
      GIT_REPOSITORY https://github.com/fmtlib/fmt
      GIT_TAG master
      GIT_SHALLOW TRUE)
    FetchContent_MakeAvailable(fmt)


add_library(slook INTERFACE)
target_include_directories(slook INTERFACE src)
target_compile_features(slook INTERFACE cxx_std_20)
target_link_libraries(slook INTERFACE slook_type_descriptors aglio)

add_library(slook::slook ALIAS slook)

add_executable(test src/test.cpp)
target_link_libraries(test slook::slook fmt::fmt)
