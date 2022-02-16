cmake_minimum_required(VERSION 3.8)

project("fractalrenderer")

find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(lodepng CONFIG REQUIRED)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin/$<0:>")

add_executable(fractalrenderer "src/fractal.cpp" "src/GLWindow.cpp" "src/shader.cpp")

target_compile_features(fractalrenderer PUBLIC cxx_std_17)
target_include_directories(fractalrenderer PRIVATE "./src")

target_link_libraries(fractalrenderer PRIVATE glad::glad)
target_link_libraries(fractalrenderer PRIVATE glfw)
target_link_libraries(fractalrenderer PRIVATE lodepng)

file(GLOB_RECURSE ALL_SHADERS "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*")

foreach(SHADER IN LISTS ALL_SHADERS)
  get_filename_component(SHADER_NAME ${SHADER} NAME)

  add_custom_command(
    TARGET fractalrenderer
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${SHADER} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/shaders/${SHADER_NAME})
endforeach()
