#include "db_connection.h"

#include <stdexcept>

#include <curl/curl.h>

/* curl_callback - handels the response from the http request
 * @contents: pointer to a buffer containing the request data
 * @size: always 1
 * @count: number of bytes in contents
 * @client_data: pointer to the client data
 *
 * Returns the number of handled bytes
 *
 * This function stores the data from curl into an string provided by the client
 *
 * This function can be called multiple timer for a single http request
 */
static size_t curl_callback(char* contents,
			    size_t size,
			    size_t count,
			    void* client_data)
{
	std::string* response = reinterpret_cast<std::string*>(client_data);

	response->append(contents, size*count);

	return size*count;
}

events::db_connection::db_connection()
{
	setup_curl();
}

std::string
events::db_connection::get_response()
{
	if (request_.empty())
		throw std::logic_error{"no request set"};

	std::string response_str;

	curl_easy_setopt(handle_, CURLOPT_WRITEDATA, &response_str);
	curl_easy_perform(handle_);

	if (response_str.empty())
		throw std::runtime_error{"empty response"};
	
	return response_str;
}

void
events::db_connection::set_request(const std::string& request)
{
	request_ = request;
	setup_curl_request();
}

std::string_view
events::db_connection::request() const
{
	return std::string_view(request_);
}

/* setup_curl - helper function for setting up curl */
void
events::db_connection::setup_curl()
{
	curl_global_init(CURL_GLOBAL_ALL);

	handle_ = curl_easy_init();

	curl_easy_setopt(handle_, CURLOPT_VERBOSE, 0L);
	curl_easy_setopt(handle_, CURLOPT_WRITEFUNCTION, curl_callback);
}

/* setup_curl_request - helper function for setting the request string*/
void
events::db_connection::setup_curl_request()
{
	curl_easy_setopt(handle_, CURLOPT_URL, request_.c_str());
}
