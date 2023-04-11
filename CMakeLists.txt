cmake_minimum_required(VERSION 3.18)

project(slook)

add_subdirectory(aglio)
add_subdirectory(recreated_goon)

recreated_goon_generate(
  slook_type_descriptors OUTPUT_DIRECTORY slook_type_descriptors FILES
  src/slook/slook_types.hpp src/slook/slook_commands.hpp)

add_library(slook INTERFACE)
target_include_directories(slook INTERFACE src)
target_compile_features(slook INTERFACE cxx_std_20)
target_link_libraries(slook INTERFACE slook_type_descriptors aglio)

add_library(slook::slook ALIAS slook)
