#define BOOST_TEST_MODULE status view test
#include <boost/test/unit_test.hpp>

#include <curses.h>

#include "status_view.h"


BOOST_AUTO_TEST_CASE(ctor_test)
{
	util::status_view status;

	BOOST_TEST(status.system_message().empty());
	BOOST_CHECK_EQUAL(status.system_time(), not_a_date_time);
}

BOOST_AUTO_TEST_CASE(system_time_test)
{
	const ptime time{date{2018, Jan, 1}};

	util::status_view status;

	status.set_system_time(time);

	BOOST_CHECK_EQUAL(status.system_time(), time);
}

BOOST_AUTO_TEST_CASE(system_message_test)
{
	constexpr wchar_t system_msg[] = L"This is a message to the system";
	
	util::status_view status;

	status.set_system_message(system_msg);

	BOOST_TEST(status.system_message().data() == system_msg);
}

BOOST_AUTO_TEST_CASE(drawing_test)
{
	constexpr wchar_t system_msg[] = L"message";
	const ptime time{date{2018, Jan, 1}};

	util::status_view status;

	status.set_system_time(time);
	status.set_system_message(system_msg);

	//s1.draw(win, 0, 0, 10, 10);
	
	BOOST_CHECK_EQUAL(status.system_time(), time);
	BOOST_TEST(status.system_message().data() == system_msg);
}
