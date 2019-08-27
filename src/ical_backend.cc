#include "ical_backend.h"

#include "parser.h"

namespace events {
using boost::posix_time::seconds;
using boost::posix_time::min_date_time;
using boost::posix_time::second_clock;

ical_backend::ical_backend(boost::asio::io_context& ctx) :
	cooldown_{1, 0, 0, 0},
	error_cooldown_{0, 10, 0, 0},
    http_client_(ctx),
	last_update_(min_date_time),
	was_error_(false)
{}

void
ical_backend::lower_cooldown()
{
	was_error_ = true;
}

void
ical_backend::set_cooldown(unsigned long secs)
{
	cooldown_ = seconds(secs);
}

void
ical_backend::set_error_cooldown(unsigned long secs)
{
	error_cooldown_ = seconds(secs);
}

void
ical_backend::set_url(std::string host, std::string target)
{
    host_ = std::move(host);
    target_ = std::move(target);
}

std::optional<std::list<events::event>>
ical_backend::update()
{
	std::string response;
	bool updated;

	try {
		response = http_client_.execute(host_, "80", target_);
		updated = true;
	} catch (...) {
		updated = false;
	}

	last_update_ = second_clock::universal_time();

	if (updated) {
		std::list<event> events;

		try {
			events = parser::events_from_ics(response);
		} catch (...) {
			return std::nullopt;
		}

		if (was_error_)
			was_error_ = false;

		return events;
	}

	return std::nullopt;
}

time_duration
ical_backend::cooldown() const
{
	return cooldown_;
}

time_duration
ical_backend::error_cooldown() const
{
	return error_cooldown_;
}

bool
ical_backend::ready() const
{
	auto diff = second_clock::universal_time() - last_update_;
	
	if (was_error_)
		return diff >= error_cooldown_;
	else
		return diff >= cooldown_;
}

std::pair<std::string_view, std::string_view>
ical_backend::url() const
{
	return {host_, target_};
}
} // namespace events
