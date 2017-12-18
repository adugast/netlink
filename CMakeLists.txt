#########
# CMAKE #
#########

cmake_minimum_required(VERSION 3.7.2)

project(netlink)

## set compilation flags
set(CMAKE_C_FLAGS "-W -Wall -Werror -pedantic")

## set the target name and source
add_executable(
    netlink
    src/main.c
)
