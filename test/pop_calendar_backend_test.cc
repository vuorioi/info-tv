#define BOOST_TEST_MODULE pop calendar backend test
#include <boost/test/unit_test.hpp>

#include <chrono>
#include <thread>

#include "pop_calendar_backend.h"

using namespace boost::gregorian;
using namespace boost::posix_time;

BOOST_AUTO_TEST_CASE(ctor)
{
	events::pop_calendar_backend backend;

	BOOST_CHECK_EQUAL(backend.cooldown(), hours(1));
	BOOST_CHECK_EQUAL(backend.error_cooldown(), minutes(10));
	BOOST_CHECK_EQUAL(backend.url(), "");
}

BOOST_AUTO_TEST_CASE(lower_cooldown_test)
{
	constexpr unsigned cooldown_secs = 2;
	constexpr unsigned error_cooldown_secs = 1;
	events::pop_calendar_backend backend;

	backend.set_cooldown(cooldown_secs);
	backend.set_error_cooldown(error_cooldown_secs);

	backend.update();

	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	BOOST_TEST(not backend.ready());

	std::this_thread::sleep_for(std::chrono::milliseconds(1500));

	BOOST_TEST(backend.ready());

	backend.update();
	backend.lower_cooldown();

	std::this_thread::sleep_for(std::chrono::milliseconds(1500));

	BOOST_TEST(backend.ready());
}

BOOST_AUTO_TEST_CASE(set_cooldown_value_test)
{
	events::pop_calendar_backend backend;

	backend.set_cooldown(600);

	BOOST_CHECK_EQUAL(backend.cooldown(), minutes(10));
}

BOOST_AUTO_TEST_CASE(set_error_cooldown_value_test)
{
	events::pop_calendar_backend backend;

	backend.set_error_cooldown(60);

	BOOST_CHECK_EQUAL(backend.error_cooldown(), minutes(1));
}

BOOST_AUTO_TEST_CASE(set_url_test)
{
	constexpr char url[] = "http://www.google.com";

	events::pop_calendar_backend backend;

	backend.set_url(url);

	BOOST_TEST(backend.url() == url);
}

BOOST_AUTO_TEST_CASE(update_test)
{
	events::pop_calendar_backend backend;

	BOOST_CHECK_NO_THROW(backend.update());
}

BOOST_AUTO_TEST_CASE(ready_test)
{
	events::pop_calendar_backend backend;

	BOOST_TEST(backend.ready());

	backend.update();

	BOOST_TEST(not backend.ready());
}
