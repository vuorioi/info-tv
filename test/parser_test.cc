#define BOOST_TEST_MODULE event parser test
#include <boost/test/unit_test.hpp>

#include <cerrno>
#include <fstream>
#include <list>
#include <string>

#include "event.h"
#include "parser.h"

static std::string read_json(const char *filename)
{
	std::string contents;
	std::ifstream in(filename, std::ios::in | std::ios::binary);

	if (in) {
		in.seekg(0, std::ios::end);
		contents.resize(in.tellg());

		in.seekg(0, std::ios::beg);

		in.read(&contents[0], contents.size());

		in.close();
	} else {
		throw errno;
	}

	return contents;
}

BOOST_AUTO_TEST_CASE(malformed_json)
{
	//// Case 0: malformed json
	BOOST_CHECK_THROW(events::parser::events_from_json("this is wrong"),
			  std::runtime_error);
}

BOOST_AUTO_TEST_CASE(malformed_header_no_kind)
{
	//// Case 0: malformed header: no kind
	BOOST_CHECK_THROW(events::parser::events_from_json(
			  read_json("../test/test_json/no_kind.json")),
			  std::runtime_error);
}
	
BOOST_AUTO_TEST_CASE(malformed_header_wrong_kind)
{
	//// Case 1: malformed header: wrong kind
	BOOST_CHECK_THROW(events::parser::events_from_json(
			  read_json("../test/test_json/wrong_kind.json")),
			  std::runtime_error);
}
	
BOOST_AUTO_TEST_CASE(malformed_header_no_events)
{
	std::list<events::event> l;

	//// Case 2: no events
	l = events::parser::events_from_json(read_json("../test/test_json/no_events.json"));

	BOOST_TEST(l.size() == 0);
}

BOOST_AUTO_TEST_CASE(malformed_events_kind)
{
	//// Case 0: malformed event: no kind
	BOOST_CHECK_THROW(events::parser::events_from_json(
			  read_json("../test/test_json/event_no_kind.json")),
			  std::runtime_error);

	//// Case 1: malformed event: wrong kind
	BOOST_CHECK_THROW(events::parser::events_from_json(
			  read_json("../test/test_json/event_wrong_kind.json")),
			  std::runtime_error);
}
	
BOOST_AUTO_TEST_CASE(malformed_events_status)
{
	//// Case 2: malformed event: no status
	BOOST_CHECK_THROW(events::parser::events_from_json(
			  read_json("../test/test_json/event_no_status.json")),
			  std::runtime_error);
	
	//// Case 3: malformed event: wrong status
	BOOST_CHECK_THROW(events::parser::events_from_json(
			  read_json("../test/test_json/event_wrong_status.json")),
			  std::runtime_error);
}

BOOST_AUTO_TEST_CASE(malformed_events_summary)
{
	//// Case 4: malformed event: no summary
	BOOST_CHECK_THROW(events::parser::events_from_json(
			  read_json("../test/test_json/event_no_summary.json")),
			  std::runtime_error);
}
	
BOOST_AUTO_TEST_CASE(malformed_events_start)
{
	//// Case 5: malformed event: no start
	BOOST_CHECK_THROW(events::parser::events_from_json(
			  read_json("../test/test_json/event_no_start.json")),
			  std::runtime_error);

	//// Case 6: malformed event: no dateTime in start
	BOOST_CHECK_THROW(events::parser::events_from_json(
			  read_json("../test/test_json/event_start_no_datetime.json")),
			  std::runtime_error);

	//// Case 7: malformed event: wrong dateTime format in start
	BOOST_CHECK_THROW(events::parser::events_from_json(
			  read_json("../test/test_json/event_start_wrong_format_datetime.json")),
			  std::runtime_error);
	//TODO tests for date in start and format
}
	
BOOST_AUTO_TEST_CASE(malformed_events_end)
{
	//// Case 8: malformed event: no end
	BOOST_CHECK_THROW(events::parser::events_from_json(
			  read_json("../test/test_json/event_no_end.json")),
			  std::runtime_error);
	
	//// Case 9: malformed event: no dateTime in start
	BOOST_CHECK_THROW(events::parser::events_from_json(
			  read_json("../test/test_json/event_end_no_datetime.json")),
			  std::runtime_error);
	
	//// Case 10: malformed event: wrong dateTime format in start
	BOOST_CHECK_THROW(events::parser::events_from_json(
			  read_json("../test/test_json/event_end_wrong_format_datetime.json")),
			  std::runtime_error);
	//TODO tests for date in start and format
}

BOOST_AUTO_TEST_CASE(valid_json)
{
	const time_period first_duration{ptime{date{2018, Jan, 1},
					 hours(16) + minutes(30)},
					 ptime{date{2018, Jan, 2},
					 hours(4) + minutes(15) + seconds(20)}};
	const time_period last_duration{ptime{date{2018, Jan, 2}},
					ptime{date{2018, Jan, 4}}};

	std::list<events::event> l;

	l = events::parser::events_from_json(read_json("../test/test_json/working.json"));

	BOOST_TEST(l.size() == 2);

	BOOST_TEST(l.front().name().data() == L"Event 1");
	BOOST_TEST(l.front().description().data() == L"First event");
	BOOST_TEST(l.front().location().data() == L"Location A");
	BOOST_CHECK_EQUAL(l.front().duration(), first_duration);

	BOOST_TEST(l.back().name().data() == L"Event 2");
	BOOST_TEST(l.back().description().data() == L"Later event");
	BOOST_TEST(l.back().location().data() == L"Location B");
	BOOST_CHECK_EQUAL(l.back().duration(), last_duration);
}

BOOST_AUTO_TEST_CASE(tentative_event)
{
	std::list<events::event> l;

	l = events::parser::events_from_json(read_json("../test/test_json/tentative_event.json"));

	BOOST_TEST(l.size() == 0);
}
