#pragma once

#include <mutex>
#include <string>
#include <string_view>

#include <boost/asio.hpp>


namespace motd {

class motd_model {
    mutable bool changed_;
    boost::asio::io_context& ctx_;
    std::wstring message_;
    mutable std::mutex message_mtx_;
    std::string password_;

public:
    motd_model(boost::asio::io_context& ctx,
               unsigned port,
               std::string password);
    motd_model(const motd_model& other) = delete;
    motd_model(motd_model&& other) = delete;

    bool changed() const;
    bool empty() const;
    std::wstring message() const;
    bool message(std::string tcp_msg);
    motd_model& operator=(const motd_model& rhs) = delete;
    motd_model& operator=(motd_model&& rhs) = delete;
};

} // namespace motd
