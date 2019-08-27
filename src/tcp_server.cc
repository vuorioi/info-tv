#include "tcp_server.h"

#include <stdexcept>


namespace motd {
tcp_server::tcp_server(boost::asio::io_context& ctx,
                       unsigned port,
                       std::function<bool(std::string)> handler) :
    acceptor_(ctx,
              boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)),
    handler_(std::move(handler))
{}

void
tcp_server::run()
{
    do_accept();
}

void
tcp_server::do_accept()
{
    acceptor_.async_accept(std::bind(&tcp_server::on_accept,
                                     shared_from_this(),
                                     std::placeholders::_1,
                                     std::placeholders::_2));
}

void
tcp_server::on_accept(const boost::system::error_code& error,
                      boost::asio::ip::tcp::socket socket)
{
    if (error)
        throw std::runtime_error("do accept error");

    std::make_shared<tcp_connection>(std::move(socket),
                                     handler_)->run();

    do_accept();
}
} // namespace motd
