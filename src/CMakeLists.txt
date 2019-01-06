set(COMPILE_DEFINITIONS -Werror)

find_package(nlohmann_json 3.2.0 REQUIRED)

find_package(Boost REQUIRED COMPONENTS date_time)

include_directories(${Boost_INCLUDE_DIRS})

add_library(Utility
	utility.cc)

add_library(Ui
	ui.cc)
target_link_libraries(Ui
	ncursesw)

add_library(iCalendar
	icalendar.cc)

add_library(Event
	db_connection.cc
	event.cc
	event_model.cc
	event_view.cc
	google_calendar_backend.cc
	parser.cc
	pop_calendar_backend.cc)
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
	Event
	Status
	Ui
	Utility)

install(TARGETS info-tv
	RUNTIME DESTINATION bin)

install(DIRECTORY ${InfoTV_SOURCE_DIR}/media/
	DESTINATION share/info-tv)
