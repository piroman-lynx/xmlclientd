cmake_minimum_required (VERSION 2.6) 
project(xmlclientd)

include(CheckFunctionExists)
include(CheckIncludeFiles)
INCLUDE (CheckLibraryExists) 

#CHECK_LIBRARY_EXISTS(glib "g_hash_table_new" "/usr/include/glib-2.0/" HAVE_GLIB20)

#IF(NOT HAVE_GLIB20)
#    MESSAGE(FATAL_ERROR "ERROR: unable to link glib 2.0")
#ENDIF(NOT HAVE_GLIB20)

include_directories( ${CMAKE_CURRENT_BINARY_DIR} "/usr/include/glib-2.0/" "/usr/lib/x86_64-linux-gnu/glib-2.0/include/"  )

set(CMAKE_EXTRA_INCLUDE_FILES unistd.h stdlib.h stdio.h)

CHECK_FUNCTION_EXISTS(fork HAVE_FORK)
CHECK_FUNCTION_EXISTS(vfork HAVE_VFORK)
CHECK_FUNCTION_EXISTS(printf HAVE_PRINTF)
CHECK_FUNCTION_EXISTS(malloc HAVE_MALLOC)
CHECK_FUNCTION_EXISTS(free HAVE_FREE)
CHECK_FUNCTION_EXISTS(strstr HAVE_STRSTR)
CHECK_FUNCTION_EXISTS(accept HAVE_ACCEPT)
CHECK_FUNCTION_EXISTS(getnameinfo HAVE_GETNAMEINFO)
CHECK_FUNCTION_EXISTS(epoll_wait HAVE_EPOLL_WAIT)
CHECK_FUNCTION_EXISTS(epoll_ctl HAVE_EPOLL)
CHECK_FUNCTION_EXISTS(fcntl HAVE_FCNTL)
CHECK_FUNCTION_EXISTS(memset HAVE_MEMSET)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GLIB_PKG glib-2.0)

set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} "/usr/lib/x86_64-linux-gnu/")

add_executable (bin/xmlclientd src/xmlclientd.c src/process.c src/master.c src/debug.c src/client.c src/openproto.c src/strpos.c src/console.c src/server.c src/network.c src/url.c src/const.c)
add_executable (bin/xmlclientc src/xmlclientc.c src/process.c src/master.c src/debug.c src/client.c src/openproto.c src/strpos.c src/console.c src/server.c src/network.c src/url.c src/const.c)

target_link_libraries (bin/xmlclientd glib-2.0)
target_link_libraries (bin/xmlclientc glib-2.0)

#/usr/lib/x86_64-linux-gnu/libglib
