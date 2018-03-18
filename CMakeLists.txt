cmake_minimum_required (VERSION 3.5)

project(a_stream)
add_definitions(-Wall)

add_executable(server server.c common.c)
add_executable(client client.c common.c)