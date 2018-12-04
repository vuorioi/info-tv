#define BOOST_TEST_MODULE db connection test
#include <boost/test/unit_test.hpp>

#include "db_connection.h"

BOOST_AUTO_TEST_CASE(ctor_test)
{
	events::db_connection c1;

	BOOST_TEST(c1.request().length() == 0);
}

BOOST_AUTO_TEST_CASE(set_request_test)
{
	constexpr char request[] = "http://www.test.com";
	events::db_connection c1;

	c1.set_request(request);

	BOOST_TEST(c1.request().data() == request);
}

BOOST_AUTO_TEST_CASE(get_response_test)
{
	std::string response;

	//// Case 0: no request set
	events::db_connection c1;

	BOOST_CHECK_THROW(response = c1.get_response(), std::logic_error);

	//// Case 1: request set
	constexpr char request[] = "http://www.google.com/";
	events::db_connection c2;

	c2.set_request(request);

	response = c2.get_response();

	BOOST_TEST(response.length() != 0);
}
