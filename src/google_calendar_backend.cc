#include "google_calendar_backend.h"

#include "parser.h"

namespace events {
using boost::posix_time::seconds;
using boost::posix_time::min_date_time;
using boost::posix_time::second_clock;

google_calendar_backend::google_calendar_backend(boost::asio::io_context& ctx) :
	cooldown_{1, 0, 0, 0},
	error_cooldown_{0, 10, 0, 0},
    http_client_(ctx),
	last_update_{min_date_time},
	was_error_{false}
{}

void
google_calendar_backend::lower_cooldown()
{
	was_error_ = true;
}

void
google_calendar_backend::set_cooldown(unsigned long secs)
{
	cooldown_ = seconds(secs);
}

void
google_calendar_backend::set_error_cooldown(unsigned long secs)
{
	error_cooldown_ = seconds(secs);
}

void
google_calendar_backend::set_id(const std::string& id)
{
	id_ = id;
}

void
google_calendar_backend::set_key(const std::string& key)
{
	key_ = key;
}

std::optional<std::list<events::event>>
google_calendar_backend::update()
{
	// Format the Google API request using UTC time to make things easier
	auto now = second_clock::universal_time();

	auto today = now.date();
	auto time = now.time_of_day();

    std::string host = "www.googleapis.com";

	std::stringstream target_ss;
	target_ss << "/calendar/v3/calendars/"
              << id_
              << "/events?"
              << "timeMin="
              << today.year()
	          << '-'
		      << std::setw(2)
		      << std::setfill('0')
		      << int(today.month())
		      << '-'
		      << std::setw(2)
		      << std::setfill('0')
		      << today.day()
		      << 'T'
		      << std::setw(2)
		      << std::setfill('0')
		      << time.hours()
		      << ':'
		      << std::setw(2)
		      << std::setfill('0')
		      << time.minutes()
		      << ':'
		      << std::setw(2)
		      << std::setfill('0')
		      << time.seconds()
		      << 'Z'
		      << "&orderBy=startTime"
		      << "&singleEvents=true"
		      << "&key="
		      << key_;

	std::string response;
	bool success;

	try {
		response = http_client_.execute(host, "80", target_ss.str());
		success = true;
	} catch (...) {
		success = false;
	}

	last_update_ = second_clock::universal_time();

	if (success) {
		std::list<event> events;

		try {
			events = parser::events_from_json(response);
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
google_calendar_backend::cooldown() const
{
	return cooldown_;
}

time_duration
google_calendar_backend::error_cooldown() const
{
	return error_cooldown_;
}

std::string_view
google_calendar_backend::id() const
{
	return std::string_view{id_};
}

std::string_view
google_calendar_backend::key() const
{
	return std::string_view{key_};
}

bool
google_calendar_backend::ready() const
{
	auto diff = second_clock::universal_time() - last_update_;
	
	if (was_error_)
		return diff >= error_cooldown_;
	else
		return diff >= cooldown_;
}
} // namespace events
