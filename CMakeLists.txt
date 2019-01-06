cmake_minimum_required(VERSION 3.5.1)

project(InfoTV)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(src)
add_subdirectory(test)

enable_testing()
add_test(NAME event COMMAND event_test)
add_test(NAME db_connection COMMAND db_connection_test)
add_test(NAME parser COMMAND parser_test)
add_test(NAME status_view COMMAND status_view_test)
add_test(NAME google_calendar_backend COMMAND google_calendar_backend_test)
add_test(NAME pop_calendar_backend COMMAND pop_calendar_backend_test)
