#define BOOST_TEST_MODULE event test
#include <boost/test/included/unit_test.hpp>
#include <boost/date_time.hpp>

#include "event.h"

using namespace boost::gregorian;
using namespace boost::posix_time;

BOOST_AUTO_TEST_CASE(ctor_test)
{
	constexpr wchar_t event_name[] = L"event name";

	const ptime start{date{2018, Jan, 1}, hours{12}};
	const ptime end{date{2018, Jan, 2}, hours{16} + minutes{30}};

	// Case 0: invalid duration
	BOOST_CHECK_THROW(events::event(event_name, end, start),
			  std::logic_error);

	// Case 0: valid duration
	events::event event{event_name, start, end};

	BOOST_TEST((event.duration() == time_period(start, end)));
	BOOST_TEST(event.name().data() == event_name);
	BOOST_TEST(event.location().data() == L"");
}

BOOST_AUTO_TEST_CASE(set_duration_test)
{
	constexpr wchar_t event_name[] = L"event name";

	const ptime start1{date{2018, Jan, 2}, hours{14}};
	const ptime end1{date{2018, Jan, 4}, hours{8} + minutes{45}};
	const ptime start2{date{2018, Nov, 1}, hours{12} + minutes{30}};
	const ptime end2{date{2018, Nov, 15}, hours{8}};
	
	events::event event1{event_name,
			     start1,
			     end1};
	
	// Case 0: invalid duration
	BOOST_CHECK_THROW(event1.set_duration(end1, start1),
			  std::logic_error);
	
	BOOST_CHECK_EQUAL(event1.duration(), time_period(start1, end1));

	// Case 1: valid duration
	event1.set_duration(start2, end2);

	BOOST_CHECK_EQUAL(event1.duration(), time_period(start2, end2));
}

BOOST_AUTO_TEST_CASE(set_id)
{
	constexpr wchar_t event_name[] = L"event name";
	constexpr char id[] = "X13fs4g";

	const ptime start{date{2018, Jan, 2}, hours{14}};
	const ptime end{date{2018, Jan, 4}, hours{8} + minutes{45}};

	events::event event{event_name,
			    start,
			    end};

	event.set_id(id);

	BOOST_TEST(event.id().data() == id);
}
	
BOOST_AUTO_TEST_CASE(set_name_test)
{
	constexpr wchar_t event_name1[] = L"event name";
	constexpr wchar_t event_name2[] = L"another name";

	const ptime start{date{2018, Jan, 2}, hours{14}};
	const ptime end{date{2018, Jan, 4}, hours{8} + minutes{45}};

	events::event event1{event_name1,
			     start,
			     end};

	event1.set_name(event_name2);

	BOOST_TEST(event1.name().data() == event_name2);
}

BOOST_AUTO_TEST_CASE(set_location_test)
{
	constexpr wchar_t event_name[] = L"event name";
	constexpr wchar_t event_location1[] = L"event location";
	constexpr wchar_t event_location2[] = L"new location";

	const ptime start{date{2018, Jan, 2}, hours{14}};
	const ptime end{date{2018, Jan, 4}, hours{8} + minutes{45}};

	events::event event1{event_name,
			     start,
			     end};

	event1.set_location(event_location1);

	BOOST_TEST(event1.location().data() == event_location1);

	event1.set_location(event_location2);

	BOOST_TEST(event1.location().data() == event_location2);
}
