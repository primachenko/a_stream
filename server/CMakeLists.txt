cmake_minimum_required (VERSION 3.5)

project(server)
add_definitions(-Wall)

add_executable(server server.c net.c)

target_link_libraries(server
	pulse
	pulse-simple)