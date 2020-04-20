cmake_minimum_required(VERSION 3.1.0)

project(TCP-Server LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(project_sources
    server.cpp)

add_executable(${PROJECT_NAME} ${project_sources})