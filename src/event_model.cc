#include "event_model.h"

#include <algorithm>
#include <iterator>
#include <sstream>

#include "parser.h"

events::event_model::event_model(const std::string& calendar_id,
				 const std::string& api_key,
				 const long cooldown_seconds) :
	cooldown_{0, 0, cooldown_seconds, 0},
	id_{calendar_id},
	key_{api_key}

{
	last_update_ = second_clock::universal_time() - cooldown_;
}

bool
events::event_model::update()
{
	auto now = second_clock::universal_time();

	if (cooldown_ <= now - last_update_) {
		auto today = now.date();
		auto time = now.time_of_day();

		// Format the request string using UTC time to make things easier
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

		try {
			response = db_.get_response();
		} catch (const std::runtime_error& e) {
			return false;
		}

		last_update_ = second_clock::universal_time();

		try {
			events_ = parser::events_from_json(response);
		} catch (const std::runtime_error& e) {
			return false;
		}
	} else {
		// Remove events that have already ended
		events_.remove_if([](const events::event& e) {
			return e.duration().end() < second_clock::local_time();
		});
	}

	return true;
}

std::list<events::event>
events::event_model::events() const
{
	return events_;
}
