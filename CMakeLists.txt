cmake_minimum_required (VERSION 2.6) 
project(xmlclientd)

add_executable (xmlclientd xmlclientd.c process.c master.c debug.c client.c openproto.c strpos.c console.c)

