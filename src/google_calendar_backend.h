#pragma once

#include <list>
#include <optional>
#include <string>
#include <string_view>

#include <boost/date_time.hpp>

#include "db_connection.h"
#include "event.h"
#include "event_backend_interface.h"

using boost::posix_time::ptime;
using boost::posix_time::time_duration;

namespace events {
class google_calendar_backend : public event_backend_interface {
public:
	/* ctor
	 *
	 * This ctor creates an Google Calendar backend object with cooldown
	 * period of 1 hour and an error cooldown of 10 minutes
	 */
	google_calendar_backend();
	/* explicitly deleted copy ctor */
	google_calendar_backend(const google_calendar_backend& rhs) = delete;
	
	/* explicitly defaulted copy ctor */
	virtual ~google_calendar_backend() = default;

	/* lower_cooldown - implemented from the event_backend_interface */
	virtual void lower_cooldown() override;
	/* set_cooldown - sets the normal cooldown value
	 * @seconds: cooldown in seconds
	 */
	virtual void set_cooldown(unsigned long seconds);
	/* set_error_cooldown - sets the error cooldown value
	 * @seconds: error cooldown in seconds
	 */
	virtual void set_error_cooldown(unsigned long seconds);
	/* set_id - sets the calendar id
	 * @id: calendar id
	 *
	 * Set the id for the Google Calendar to use.
	 */
	virtual void set_id(const std::string& id);
	/* set_key - sets the API key
	 * @key: API key
	 *
	 * Sets the Google API key that can be used to access the Calendar.
	 */
	virtual void set_key(const std::string& key);
	/* update - implemented from the event_backend_interface */
	virtual std::optional<std::list<event>> update() override;

	/* cooldown - get the cooldown value
	 *
	 * Returns boost::posix_time::time_duration representation of the
	 * cooldown.
	 */
	virtual time_duration cooldown() const;
	/* error_cooldown - ge the error cooldown value
	 *
	 * Returns boost::posix_time::time_duration representation of the 
	 * error cooldown value.
	 */
	virtual time_duration error_cooldown() const;
	/* id - get the calendar id
	 *
	 * Returns a string view to the calendar id
	 */
	virtual std::string_view id() const;
	/* key - get the API key
	 *
	 * Returns a string view to the API key
	 */
	virtual std::string_view key() const;
	/* ready - implemented from the event_backend_interface */
	virtual bool ready() const override;

protected:
	time_duration cooldown_;
	db_connection db_;
	time_duration error_cooldown_;
	std::string id_;
	std::string key_;
	ptime last_update_;
	bool was_error_;
};
}
