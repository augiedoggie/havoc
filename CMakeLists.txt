cmake_minimum_required(VERSION 3.19)

project(MKH)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/CMakeModules")

include(UseHaiku)

add_subdirectory(Source)
