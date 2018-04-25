cmake_minimum_required (VERSION 3.5)

add_definitions(-Wall)

set(SRC
	client.c)

set(LIBS
	pulse
	pulse-simple)

add_executable(client ${SRC})