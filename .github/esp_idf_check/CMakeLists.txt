# Minimal ESP-IDF project for the CI compile check (not for distribution).
# The repository root is the M5Utility component under test; 'main' auto-requires
# every discovered component, so no name-based reference is needed
cmake_minimum_required(VERSION 3.16)
list(APPEND EXTRA_COMPONENT_DIRS "${CMAKE_CURRENT_LIST_DIR}/../..")
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(m5utility_compile_check)
