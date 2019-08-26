#include "tcp_server.h"

#include <stdexcept>


namespace motd {
tcp_server::tcp_server(boost::asio::io_service& ctx,
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
    auto connection =
        std::make_shared<tcp_connection>(acceptor_.get_io_service(),
                                         handler_);

    acceptor_.async_accept(connection->socket(),
                           std::bind(&tcp_server::on_accept,
                                     shared_from_this(),
                                     std::placeholders::_1,
                                     connection));
}

void
tcp_server::on_accept(const boost::system::error_code& error,
                      std::shared_ptr<tcp_connection> connection)
{
    if (error)
        //return tcp_fail("do_accept", error); //FIXME throw
        throw std::runtime_error("do accept error");

    connection->run();

    do_accept();
}
} // namespace motd
