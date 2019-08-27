#pragma once

#include <memory>

#include <boost/asio.hpp>


namespace motd {

class tcp_connection : public std::enable_shared_from_this<tcp_connection> {
    std::function<bool(std::string)> handler_;
    std::string response_;
    boost::asio::ip::tcp::socket socket_;
    boost::asio::streambuf streambuf_;

public:
    tcp_connection(boost::asio::ip::tcp::socket socket,
                   std::function<bool(std::string)> handler);
    tcp_connection(const tcp_connection& other) = delete;
    tcp_connection(tcp_connection&& other) = delete;

    tcp_connection& operator=(const tcp_connection& rhs) = delete;
    tcp_connection& operator=(tcp_connection&& rhs) = delete;
    void run();
    boost::asio::ip::tcp::socket& socket();

private:
    void do_read();
    void do_write(std::string response);
    void on_read(const boost::system::error_code& error,
                 size_t bytes_transferred);
    void on_write(const boost::system::error_code& error,
                  size_t bytes_transferred);
};

} // namespace motd
