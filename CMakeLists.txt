cmake_minimum_required (VERSION 2.6) 
project(xmlclientd)

include(CheckFunctionExists)
include(CheckIncludeFiles)
INCLUDE (CheckLibraryExists) 


set(CMAKE_EXTRA_INCLUDE_FILES unistd.h stdlib.h stdio.h)
CHECK_FUNCTION_EXISTS(fork HAVE_FORK)
CHECK_FUNCTION_EXISTS(vfork HAVE_VFORK)
CHECK_FUNCTION_EXISTS(printf HAVE_PRINTF)
CHECK_FUNCTION_EXISTS(malloc HAVE_MALLOC)
CHECK_FUNCTION_EXISTS(free HAVE_FREE)
CHECK_FUNCTION_EXISTS(strstr HAVE_STRSTR)


add_executable (xmlclientd xmlclientd.c process.c master.c debug.c client.c openproto.c strpos.c console.c server.c network.c)

