cmake_minimum_required(VERSION 3.1.0)

project(TCP-Server LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(project_headers
	server.h)

set(project_sources
	server.cpp)

add_executable(${PROJECT_NAME} ${project_headers} ${project_sources})

target_link_libraries(${PROJECT_NAME}
	Ws2_32.lib)