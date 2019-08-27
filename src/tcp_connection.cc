#include "tcp_connection.h"

#include <array>
#include <iostream>
#include <memory>

namespace motd {
//FIXME remove
static void tcp_fail(std::string what, boost::system::error_code code);

tcp_connection::tcp_connection(boost::asio::ip::tcp::socket socket,
                               std::function<bool(std::string)> handler) :
    handler_(std::move(handler)),
    socket_(std::move(socket))
{}

void
tcp_connection::run()
{
    do_read();
}

boost::asio::ip::tcp::socket&
tcp_connection::socket()
{
    return socket_;
}

void
tcp_connection::do_read()
{
    boost::asio::async_read_until(socket_,
                                  streambuf_,
                                  '\n',
                                  std::bind(&tcp_connection::on_read,
                                            shared_from_this(),
                                            std::placeholders::_1,
                                            std::placeholders::_2));
}

void
tcp_connection::do_write(std::string response)
{
    response_ = std::move(response);

    boost::asio::async_write(socket_,
                             boost::asio::buffer(response_),
                             std::bind(&tcp_connection::on_write,
                                       shared_from_this(),
                                       std::placeholders::_1,
                                       std::placeholders::_2));
}

void
tcp_connection::on_read(const boost::system::error_code& error,
                        size_t bytes_transferred)
{
    if (error)
        return tcp_fail("on_read", error); //FIXME throw

    std::string message{boost::asio::buffers_begin(streambuf_.data()),
                        boost::asio::buffers_begin(streambuf_.data()) +
                        bytes_transferred - 1};

    if (handler_(std::move(message)))
        do_write("+\n");
    else
        do_write("-\n");
}

void
tcp_connection::on_write(const boost::system::error_code& error,
                         size_t /*unused*/)
{
    socket_.close();
}

//FIXME remove
static void tcp_fail(std::string what, boost::system::error_code code)
{
    std::cerr << what << ' ' << code.message() << '\n';
}
} // namespace motd
