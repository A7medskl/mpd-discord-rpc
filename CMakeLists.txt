cmake_minimum_required(VERSION 3.6)
project(mpd-discord-rpc)

set(SOURCES
  src/main.c
  src/mpdclient.c
  )

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} mpdclient)
