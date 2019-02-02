#include "ical_backend.h"

#include "parser.h"

using boost::posix_time::seconds;
using boost::posix_time::min_date_time;
using boost::posix_time::second_clock;

events::pop_calendar_backend::pop_calendar_backend() :
	cooldown_{1, 0, 0, 0},
	error_cooldown_{0, 10, 0, 0},
	last_update_{min_date_time},
	was_error_{false}
{}

void
events::pop_calendar_backend::lower_cooldown()
{
	was_error_ = true;
}

void
events::pop_calendar_backend::set_cooldown(unsigned long secs)
{
	cooldown_ = seconds(secs);
}

void
events::pop_calendar_backend::set_error_cooldown(unsigned long secs)
{
	error_cooldown_ = seconds(secs);
}

void
events::pop_calendar_backend::set_url(const std::string& url)
{
	url_ = url;
}

std::optional<std::list<events::event>>
events::pop_calendar_backend::update()
{
	db_.set_request(url_);

	std::string response;
	bool updated;

	try {
		response = db_.get_response();
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
events::pop_calendar_backend::cooldown() const
{
	return cooldown_;
}

time_duration
events::pop_calendar_backend::error_cooldown() const
{
	return error_cooldown_;
}

bool
events::pop_calendar_backend::ready() const
{
	auto diff = second_clock::universal_time() - last_update_;
	
	if (was_error_)
		return diff >= error_cooldown_;
	else
		return diff >= cooldown_;
}

std::string_view
events::pop_calendar_backend::url() const
{
	return std::string_view{url_};
}


