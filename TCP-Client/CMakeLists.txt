cmake_minimum_required(VERSION 3.1.0)

project(TCP-Client LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(project_sources
    client.cpp)

add_executable(${PROJECT_NAME} ${project_sources})