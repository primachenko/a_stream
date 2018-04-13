cmake_minimum_required (VERSION 3.5)

project(client)
add_definitions(-Wall)

add_executable(client
	client.c
	codec.c
	handlers.c
	net.c
	sound.c)

target_link_libraries(client
	avcodec
	avutil
	pthread
	pulse
	pulse-simple)