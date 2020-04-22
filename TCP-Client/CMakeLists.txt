cmake_minimum_required(VERSION 3.1.0)

project(TCP-Client LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(project_headers
	client.h)

set(project_sources
	client.cpp)

add_executable(${PROJECT_NAME} ${project_headers} ${project_sources})

target_link_libraries(${PROJECT_NAME}
	Ws2_32.lib
	Mswsock.lib
	AdvApi32.lib)