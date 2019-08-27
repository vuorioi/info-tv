#pragma once

#include <string>

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <curl/curl.h>


namespace events {
class http_get {
    //boost::asio::ip::tcp::resolver resolver_;
    //boost::beast::tcp_stream stream_;
    CURL* handle_;
    std::string request_;

public:
    http_get(boost::asio::io_context& ctx);
    http_get(const http_get& other) = delete;
    http_get(http_get&& other) = delete;

    std::string execute(std::string host, std::string port, std::string target);
    http_get& operator=(const http_get& rhs) = delete;
    http_get& operator=(http_get&& rhs) = delete;


private:
    void setup_curl();
    void setup_curl_request();

    static size_t curl_callback(char* contents,
                                size_t size,
                                size_t count,
                                void* client_data);
};
} // namespace events
