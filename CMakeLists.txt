#specify themminimum cmake version to use
cmake_minimum_required(VERSION 3.2)

#Specify your project name
project(
  WebRtcClientWrapper
  VERSION 0.1
  DESCRIPTION "WebRtcClientWrapper client project"
  LANGUAGES CXX)

#find the library to link with current lib
find_package(Threads)
#Add your header directory
# set(HEADERS "inc")
# set(HEADERS "~/webrtc-checkout/src")
# Add the header directories
include_directories(
    inc
    ~/webrtc-checkout/src
)

# Add your source files
set(SOURCES
    client/webRTC_client_wrapper.cpp
)

#Specify the location of the precompiled static library:
link_directories("lib")
list(APPEND webrtc_libs "webrtc")
list(APPEND webrtc_libs "pthread")

#Building the library out of source code
add_library(libWebRtcClientWrapper ${SOURCES})

#linking the external library which is part of your system
target_link_libraries(libWebRtcClientWrapper PRIVATE ${webrtc_libs})

#include Headers files
# target_include_directories(libWebRtcClientWrapper PUBLIC ${HEADERS})

#Directory where your library is located
set_target_properties(libWebRtcClientWrapper PROPERTIES IMPORTED_LOCATION lib/libwebrtc.a)
