#include "motd_model.h"

#include <codecvt>
//#include <cstring>
#include <functional>
#include <locale>

#include "tcp_server.h"


namespace motd {
motd_model::motd_model(boost::asio::io_context& ctx,
                       unsigned port,
                       std::string password) :
    changed_(false),
    ctx_(ctx),
    password_(std::move(password))
{
    auto tcp_traffic_handler = [this](std::string tcp_msg) {
            return message(std::move(tcp_msg));
        };

    std::make_shared<tcp_server>(ctx,
                                 port,
                                 std::move(tcp_traffic_handler))->run();
}

bool
motd_model::changed() const
{
    std::lock_guard lock(message_mtx_);

    return changed_;
}

bool
motd_model::empty() const
{
    std::lock_guard lock(message_mtx_);

    return message_.empty();
}

std::wstring
motd_model::message() const
{
    std::lock_guard lock(message_mtx_);

    changed_ = false;

    return message_;
}

bool
motd_model::message(std::string tcp_msg)
{
    // TCP message format should be:
    //  <password>:<hold time>:<max 512 byte message payload>

    auto delim_0 = tcp_msg.find(':');
    auto delim_1 = tcp_msg.find(':', delim_0 + 1);

    if (delim_0 == std::string::npos or
        delim_1 == std::string::npos)
        return false;

    std::string password = tcp_msg.substr(0, delim_0);
    auto hold_time = std::stoi(tcp_msg.substr(delim_0 + 1,
                               delim_1 - delim_0));
    std::string payload = tcp_msg.substr(delim_1 + 1);

    if (password != password_)
        return false;

    if (payload.length() > 512)
        return false;

    std::lock_guard lock(message_mtx_);

	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

    message_ = converter.from_bytes(payload.c_str());
    changed_ = true;

    return true;
}
} // namespace motd
