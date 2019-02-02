#define BOOST_TEST_MODULE event test
#include <boost/test/included/unit_test.hpp>

#include "db_connection.h"
#include "event_model.h"

namespace events {
class mock_connection : public db_connection {
public:
	mock_connection();

	std::string get_response() final;
	{
		//open json test file
		//return the contents
		return json_str;
	}

	void set_request(const std::string& request) final
	{
		request_ = request;
	}

	std::string_view request() const final
	{
		return request;
	}
private:
	request
};
}

BOOST_AUTO_TEST_CASE(ctor_test)
{
	//// Case 0: no paramters
	events::event_model m1;
	
	BOOST_TEST(m1.db() == nullptr);
	
	//// Case 1: valid db connection
	events::mock_connection c;
	events::event_model m2{&c};
	
	BOOST_TEST(m2.db() == &c);

	//// Case 2: invalid db connection
	BOOST_CHECK_THROW(events::event_model m3{nullptr}, std::runtime_error);
}

BOOST_AUTO_TEST_CASE(refresh_test)
{
	//TODO how to test
}

BOOST_AUTO_TEST_CASE(iterator_test)
{
	//
}

