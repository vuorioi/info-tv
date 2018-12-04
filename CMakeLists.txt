cmake_minimum_required(VERSION 3.5.1)

project(InfoTV)

set(CMAKE_CXX_STANDARD 17)

add_custom_command(OUTPUT git_commit_rev.h
		   COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/util/git_rev_parse.sh
		   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_custom_target(git_commit_revision ALL
		  DEPENDS git_commit_rev.h)

add_subdirectory(src)
add_subdirectory(test)

enable_testing()
add_test(NAME event COMMAND event_test)
add_test(NAME db_connection COMMAND db_connection_test)
add_test(NAME parser COMMAND parser_test)
add_test(NAME status_view COMMAND status_view_test)
