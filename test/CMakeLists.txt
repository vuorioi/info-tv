find_package(Boost REQUIRED COMPONENTS unit_test_framework)

include_directories(${InfoTV_SOURCE_DIR}/src
	${Boost_INCLUDE_DIRS})

add_definitions(-DBOOST_TEST_DYN_LINK)

add_executable(event_test event_test.cc)
target_link_libraries(event_test
	Event
	${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

add_executable(db_connection_test db_connection_test.cc)
target_link_libraries(db_connection_test
	Event
	${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

add_executable(parser_test parser_test.cc)
target_link_libraries(parser_test
	Event
	${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

add_executable(status_view_test status_view_test.cc)
target_link_libraries(status_view_test
	Status
	${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
