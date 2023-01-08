cmake_minimum_required(VERSION 3.22)
project(pusn)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND) 
  message("CCACHE FOUND, SETTING COMPILER CACHING")
  set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
endif(CCACHE_FOUND)

if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zi")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
endif()

message("Adding thirdparty libraries:")
add_subdirectory(thirdparty)
message("Adding milling simulator executable")
add_subdirectory(src)

