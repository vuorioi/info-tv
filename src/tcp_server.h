#pragma once

#include <functional>
#include <memory>

#include <boost/asio.hpp>

#include "tcp_connection.h"


namespace motd {

class tcp_server : public std::enable_shared_from_this<tcp_server> {
    boost::asio::ip::tcp::acceptor acceptor_;
    std::function<bool(std::string)> handler_;

public:
    tcp_server(boost::asio::io_context& ctx,
               unsigned port,
               std::function<bool(std::string)> handler);
    tcp_server(const tcp_server& other) = delete;
    tcp_server(tcp_server&& other) = delete;

    tcp_server& operator=(const tcp_server& rhs) = delete;
    tcp_server& operator=(tcp_server&& rhs) = delete;
    void run();

private:
    void do_accept();
    void on_accept(const boost::system::error_code& error,
                   boost::asio::ip::tcp::socket socket);
};

} // namespace motd
