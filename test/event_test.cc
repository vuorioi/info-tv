#define BOOST_TEST_MODULE event test
#include <boost/test/unit_test.hpp>
#include <boost/date_time.hpp>

#include "event.h"

BOOST_AUTO_TEST_CASE(ctor_test)
{
	constexpr wchar_t event_name[] = L"event name";

	const ptime start{date{2018, Jan, 1}, hours{12}};
	const ptime end{date{2018, Jan, 2}, hours{16} + minutes{30}};

	//// Case 0: invalid duration
	BOOST_CHECK_THROW(events::event(event_name, end, start),
			  std::logic_error);

	//// Case 0: valid duration
	events::event event{event_name, start, end};

	BOOST_TEST((event.duration() == time_period(start, end)));
	BOOST_TEST(event.name().data() == event_name);
	BOOST_TEST(event.description().data() == L"");
	BOOST_TEST(event.location().data() == L"");
}

BOOST_AUTO_TEST_CASE(copy_ctor_test)
{
	//// Case 0: description and location not set
	constexpr wchar_t event_name[] = L"event name";
	constexpr wchar_t event_description[] = L"event description";
	constexpr wchar_t event_location[] = L"event location";

	const ptime start{date{2018, Jan, 2}, hours{14}};
	const ptime end{date{2018, Jan, 4}, hours{8} + minutes{45}};

	events::event event1{event_name,
			     start,
			     end};

	events::event event2{event1};

	BOOST_CHECK_EQUAL(event1.duration(), event2.duration());
	BOOST_TEST(event1.name() == event2.name());
	BOOST_TEST(event1.description() == event2.description());
	BOOST_TEST(event1.location() == event2.location());

	//// Case 1: description and location set
	events::event event3{event_name,
			     start,
			     end};

	event3.set_description(event_description);
	event3.set_location(event_location);

	events::event event4{event3};

	BOOST_CHECK_EQUAL(event3.duration(), event4.duration());
	BOOST_TEST(event3.name() == event4.name());
	BOOST_TEST(event3.description() == event4.description());
	BOOST_TEST(event3.location() == event4.location());
}

BOOST_AUTO_TEST_CASE(assignment_operator_test)
{
	constexpr wchar_t event_name1[] = L"event name";
	constexpr wchar_t event_name2[] = L"another name";
	constexpr wchar_t event_description1[] = L"event description";
	constexpr wchar_t event_description2[] = L"another description";
	constexpr wchar_t event_location1[] = L"event location";
	constexpr wchar_t event_location2[] = L"another location";

	const ptime start1{date{2018, Jan, 2}, hours{14}};
	const ptime end1{date{2018, Jan, 4}, hours{8} + minutes{45}};
	const ptime start2{date{2018, Nov, 1}, hours{12} + minutes{30}};
	const ptime end2{date{2018, Nov, 15}, hours{8}};

	events::event event1{event_name1,
			     start1,
			     end1};

	events::event event2{event_name2,
			     start2,
			     end2};

	
	event1.set_description(event_description1);
	event1.set_location(event_location1);

	event2.set_description(event_description2);
	event2.set_location(event_location2);

	//BOOST_CHECK_NOT_EQUAL(event1.duration(), event2.duration());
	BOOST_CHECK_PREDICATE(std::not_equal_to<time_period>(), (event1.duration())(event2.duration()) ); 
	BOOST_TEST(event1.name() != event2.name());
	BOOST_TEST(event1.description() != event2.description());
	BOOST_TEST(event1.location() != event2.location());
	
	event1 = event2;

	BOOST_CHECK_EQUAL(event1.duration(), event2.duration());
	BOOST_TEST(event1.name() == event2.name());
	BOOST_TEST(event1.description() == event2.description());
	BOOST_TEST(event1.location() == event2.location());
}

BOOST_AUTO_TEST_CASE(duration_test)
{
	constexpr wchar_t event_name[] = L"event name";

	const ptime start1{date{2018, Jan, 2}, hours{14}};
	const ptime end1{date{2018, Jan, 4}, hours{8} + minutes{45}};
	const ptime start2{date{2018, Nov, 1}, hours{12} + minutes{30}};
	const ptime end2{date{2018, Nov, 15}, hours{8}};
	
	events::event event1{event_name,
			     start1,
			     end1};
	
	//// Case 0: invalid duration
	BOOST_CHECK_THROW(event1.set_duration(end1, start1),
			  std::logic_error);
	
	BOOST_CHECK_EQUAL(event1.duration(), time_period(start1, end1));

	//// Case 1: valid duration
	event1.set_duration(start2, end2);

	BOOST_CHECK_EQUAL(event1.duration(), time_period(start2, end2));
}
	
BOOST_AUTO_TEST_CASE(name_test)
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

BOOST_AUTO_TEST_CASE(description_test)
{
	constexpr wchar_t event_name[] = L"event name";
	constexpr wchar_t event_description1[] = L"event description";
	constexpr wchar_t event_description2[] = L"new description";

	const ptime start{date{2018, Jan, 2}, hours{14}};
	const ptime end{date{2018, Jan, 4}, hours{8} + minutes{45}};

	events::event event1{event_name,
			     start,
			     end};

	event1.set_description(event_description1);

	BOOST_TEST(event1.description().data() == event_description1);

	event1.set_description(event_description2);

	BOOST_TEST(event1.description().data() == event_description2);
}
	
BOOST_AUTO_TEST_CASE(location_test)
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
