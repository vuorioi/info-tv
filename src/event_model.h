#pragma once

#include <list>
#include <string>

#include <boost/date_time.hpp>

#include "db_connection.h"
#include "event.h"

using namespace boost::posix_time;
using namespace boost::gregorian;

namespace events {
/* event_model class
 * This class provides data for an event_view. It gets events from a Google
 * Calendar with REST API calls and parses the resulting json.
 */
class event_model {
public:
	/* event_model - ctor
	 * @calendar_id: id of the Google Calendar to be used
	 * @api_key: API key that allows access to the calendar
	 * @cooldown_seconds: number of seconds between updates via Google API
	 */
	event_model(const std::string& calendar_id,
		    const std::string& api_key,
		    const long cooldown_seconds);
	/* event_model - explicitly deleted copy ctor */
	event_model(const event_model& rhs) = delete;
	
	/* ~event_model - explicitly defaulted dtor */
	virtual ~event_model() = default;

	/* update - try to update the model
	 *
	 * Returns true if the update was successfull false otherwise
	 *
	 * This function updates the model removing passed events and getting new
	 * ones from the Google Calendar if the cooldown period has passed
	 */
	virtual bool update();

	/* events - return the list of events
	 *
	 * Returns a list containing the events in this model
	 */
	virtual std::list<event> events() const;
	
protected:
	time_duration cooldown_;
	db_connection db_;
	std::list<event> events_;
	std::string id_;
	std::string key_;
	ptime last_update_;
};
}
