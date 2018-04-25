cmake_minimum_required (VERSION 3.5)

add_definitions(-Wall)

set(SRC
	net.c
	sound.c
	client.c
	console.c)

set(LIBS
	pthread
	pulse
	pulse-simple)

add_executable(client ${SRC})

add_subdirectory(server)

target_link_libraries(client ${LIBS})