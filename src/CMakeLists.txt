set(COMPILE_DEFINITIONS -Werror)

find_package(nlohmann_json 3.2.0 REQUIRED)

find_package(Boost REQUIRED COMPONENTS date_time)
find_package(Threads)

include_directories(${Boost_INCLUDE_DIRS})

add_library(Utility
	utility.cc)

add_library(Ui
	ui.cc)
target_link_libraries(Ui
	ncursesw)

add_library(iCalendar
	icalendar.cc)

add_library(Motd
    tcp_connection.cc
    tcp_server.cc
    motd_model.cc
    motd_view.cc)

add_library(Event
	db_connection.cc
	event.cc
	event_model.cc
	event_view.cc
	google_calendar_backend.cc
	ical_backend.cc
	parser.cc)
target_link_libraries(Event
	curl
	nlohmann_json::nlohmann_json
	Ui
	iCalendar
	${Boost_DATE_TIME_LIBRARY})

add_library(Status
	status_view.cc
	version.cc)
target_link_libraries(Status
	Ui)

add_executable(info-tv
	main.cc
	version.cc)
target_link_libraries(info-tv
    ${CMAKE_THREAD_LIBS_INIT}
	Event
    Motd
	Status
	Ui
	Utility)

install(TARGETS info-tv
	RUNTIME DESTINATION bin)

install(DIRECTORY ${InfoTV_SOURCE_DIR}/media/
	DESTINATION share/info-tv)
