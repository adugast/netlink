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
<<<<<<< HEAD
=======
    src/netlink.c
>>>>>>> 073fc2cbe89c33af0dd9e51aa8450d60b8c57608
)
