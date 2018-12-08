#include "google_calendar_backend.h"

#include "parser.h"

using boost::posix_time::seconds;
using boost::posix_time::min_date_time;
using boost::posix_time::second_clock;

events::google_calendar_backend::google_calendar_backend() :
	cooldown_{1, 0, 0, 0},
	error_cooldown_{0, 10, 0, 0},
	last_update_{min_date_time},
	was_error_{false}
{}

void
events::google_calendar_backend::lower_cooldown()
{
	was_error_ = true;
}

void
events::google_calendar_backend::set_cooldown(unsigned long secs)
{
	cooldown_ = seconds(secs);
}

void
events::google_calendar_backend::set_error_cooldown(unsigned long secs)
{
	error_cooldown_ = seconds(secs);
}

void
events::google_calendar_backend::set_id(const std::string& id)
{
	id_ = id;
}

void
events::google_calendar_backend::set_key(const std::string& key)
{
	key_ = key;
}

std::optional<std::list<events::event>>
events::google_calendar_backend::update()
{
	// Format the Google API request using UTC time to make things easier
	auto now = second_clock::universal_time();

	auto today = now.date();
	auto time = now.time_of_day();

	std::stringstream request_ss;
	request_ss << "https://www.googleapis.com/calendar/v3/calendars/"
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

	db_.set_request(request_ss.str());

	std::string response;
	bool success;

	try {
		response = db_.get_response();
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
events::google_calendar_backend::cooldown() const
{
	return cooldown_;
}

time_duration
events::google_calendar_backend::error_cooldown() const
{
	return error_cooldown_;
}

std::string_view
events::google_calendar_backend::id() const
{
	return std::string_view{id_};
}

std::string_view
events::google_calendar_backend::key() const
{
	return std::string_view{key_};
}

bool
events::google_calendar_backend::ready() const
{
	auto diff = second_clock::universal_time() - last_update_;
	
	if (was_error_)
		return diff >= error_cooldown_;
	else
		return diff >= cooldown_;
}
