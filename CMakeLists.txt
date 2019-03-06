cmake_minimum_required(VERSION 3.9)

if (${CMAKE_VERSION} VERSION_LESS 3.9)
  cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

project(mpd_discord_rpc
  VERSION 1.0
  DESCRIPTION "Discord Rich Presence for MPD"
  LANGUAGES C)

set(SOURCES
  src/main.c
  src/mpdclient.c
  src/discord.c
  )

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} mpdclient)
target_link_libraries(${PROJECT_NAME} discord-rpc)

install (TARGETS ${PROJECT_NAME} DESTINATION bin)
