cmake_minimum_required (VERSION 3.5)

project(a_stream)
add_definitions(-Wall)

# find_package(PULSE pulse)
# find_package(PULSE_SIMPLE pulse-simple)

add_executable(server server.c common.c)
add_executable(client client.c common.c)

target_link_libraries(client
	pulse
	pulse-simple)

target_link_libraries(server
	pulse
	pulse-simple)