#########
# CMAKE #
#########

cmake_minimum_required(VERSION 2.8.12)

project(netlink)

## set compilation flags
set(CMAKE_C_FLAGS "-W -Wall -pedantic -std=c99 -std=gnu99")

## set the target name and source
add_executable(
    netlink
    src/main.c
    src/netlink.c
)
