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
	bool updated;

	auto now = second_clock::universal_time();

	if (cooldown_ <= now - last_update_) {
		// Get events from both POP and Google Calendars
		constexpr char pop_request[] = "https://www.tut.fi/calendarservice/calendarfeed/events/132672/fi/3b87d99c55d40a89fa472daad50d65567670d6c4.ics";


		// Format the Google API request using UTC time to make things easier
		auto today = now.date();
		auto time = now.time_of_day();

		std::stringstream google_request_ss;
		google_request_ss << "https://www.googleapis.com/calendar/v3/calendars/"
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

		// Get responses from both calendars
		std::string pop_response;
		std::string google_response;

		db_.set_request(pop_request);
		bool pop_updated = true;

		try {
			pop_response = db_.get_response();
		} catch (...) {
			pop_updated = false;
		}


		db_.set_request(google_request_ss.str());
		bool google_updated = true;

		try {
			google_response = db_.get_response();
		} catch (...) {
			google_updated = false;
		}

		last_update_ = second_clock::universal_time();

		std::list<event> pop_events;

		if (pop_updated) {
			try {
				pop_events = parser::events_from_ics(pop_response);
			} catch (const std::runtime_error& e) {
				pop_updated = false;
			}
		} else {
			//TODO change cooldown
		}

		last_update_ = second_clock::universal_time();

		if (google_updated) {
			try {
				events_ = parser::events_from_json(response);
			} catch (const std::runtime_error& e) {
				google_updated = false;
			}
		} else {
			//TODO change cooldown
		}
	} 

	if (updated) {
		//TODO remove old pop events

		//TODO combine pop and google events

		//TODO 
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
