cmake_minimum_required(VERSION 3.25)
project(argumentum)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE debug)

include_directories(..)
include_directories(./deps)

add_executable(json
	main.cpp
	JsonArgumentHandler.h
	JsonArgumentHandler.cpp
	./deps/Jayson.h
	../Argumentum.h
)
