#include "http_get.h"

namespace events {
http_get::http_get(boost::asio::io_context& /*ctx*/)/*:
    resolver_(ctx),
    stream_(ctx)*/
{
    setup_curl();
}

std::string
http_get::execute(std::string host, std::string /*port*/, std::string target)
{
    /*if (request_.empty())
	    throw std::logic_error{"no request set"};*/

    request_ += "https://";
    request_ += host;
    request_ += target;

    setup_curl_request();

    std::string response_str;

    curl_easy_setopt(handle_, CURLOPT_WRITEDATA, &response_str);
    curl_easy_perform(handle_);

    if (response_str.empty())
        throw std::runtime_error{"empty response"};
	
    return response_str;

    /*auto const results = resolver_.resolve(host, port);

    stream_.connect(results);

    // Use HTTP version 1.1
    boost::beast::http::request<boost::beast::http::string_body>
        req{boost::beast::http::verb::get, std::move(target), 11};
    req.set(boost::beast::http::field::host, std::move(host));
    req.set(boost::beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);

    boost::beast::http::write(stream_, req);

    boost::beast::flat_buffer buffer;
    boost::beast::http::response<boost::beast::http::dynamic_body> res;

    boost::beast::http::read(stream_, buffer, res);

    boost::system::error_code ec;
    stream_.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);

    // We should not report not connected
    if(ec && ec != boost::beast::errc::not_connected)
        throw boost::beast::system_error(ec);

    return boost::beast::buffers_to_string(res.body().data());*/
}

/* setup_curl - helper function for setting up curl */
void
http_get::setup_curl()
{
    curl_global_init(CURL_GLOBAL_ALL);

    handle_ = curl_easy_init();

    curl_easy_setopt(handle_, CURLOPT_VERBOSE, 0L);
    curl_easy_setopt(handle_, CURLOPT_WRITEFUNCTION, curl_callback);
}

/* setup_curl_request - helper function for setting the request string*/
void
http_get::setup_curl_request()
{
    curl_easy_setopt(handle_, CURLOPT_URL, request_.c_str());
}

size_t
http_get::curl_callback(char* contents,
                        size_t size,
                        size_t count,
                        void* client_data)
{
    std::string* response = reinterpret_cast<std::string*>(client_data);

    response->append(contents, size*count);

    return size*count;
}
} // namespace events
