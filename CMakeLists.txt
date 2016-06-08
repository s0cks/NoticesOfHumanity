cmake_minimum_required(VERSION 3.3)
project(NoticesOfHumanity)

find_package(Threads)

include_directories(include)

add_executable(NoticesOfHumanity main.c include/noh/common.h include/noh/bot.h src/bot.c include/noh/database.h src/database.c)
target_link_libraries(NoticesOfHumanity sqlite3)
target_link_libraries(NoticesOfHumanity ${CMAKE_THREAD_LIBS_INIT})