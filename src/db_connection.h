#pragma once

#include <string>
#include <string_view>

#include <curl/curl.h>

namespace events {
/* db_connection class
 * This class wraps a curl request instance and handles the connection to
 * the Google API
 */
class db_connection {
public:

	/* db_connection - default ctor
	 *
	 * Constructs a connection object setting up the underlaying curl
	 * interface
	 */
	db_connection();
	/* db_connection - explicitly deleted copy ctor */
	db_connection(const db_connection& rhs) = delete;

	/* ~db_connection - explicitly defaulted dtor */
	~db_connection() = default;

	/* get_response - perform the request
	 * 
	 * Returns a string containing the response. Throws std::logic_error
	 * if the request was empty or std::runtime_error if the response was
	 * empty
	 *
	 * This call blocks until curl has performed the http reequest!
	 */
	std::string get_response();
	/* set_request - sets the request string
	 * @request: the string to use as request
	 *
	 * The string is used as the url of the http request
	 */
	void set_request(const std::string& request);

	/* request - returns the request string
	 *
	 * Returns a string view  to the string used as the request string
	 */
	std::string_view request() const;

protected:
	CURL* handle_;
	std::string request_;

	void setup_curl();
	void setup_curl_request();
};
}
