#pragma once

#include <list>
#include <optional>
#include <string>
#include <string_view>

#include <boost/asio.hpp>
#include <boost/date_time.hpp>

#include "event.h"
#include "event_backend_interface.h"
#include "http_get.h"

using boost::posix_time::ptime;
using boost::posix_time::time_duration;

namespace events {
/* google_calendar_backend class
 * This class provides an event backend to the Google Calendars.
 */
class google_calendar_backend : public event_backend_interface {
protected:
	time_duration cooldown_;
	time_duration error_cooldown_;
    http_get http_client_;
	std::string id_;
	std::string key_;
	ptime last_update_;
	bool was_error_;

public:
	/* ctor
     * @ctx: io_context for the http client code
	 *
	 * This ctor creates a Google Calendar backend object with cooldown
	 * period of 1 hour and an error cooldown of 10 minutes.
	 */
	google_calendar_backend(boost::asio::io_context& ctx);
	/* explicitly deleted copy ctor */
	google_calendar_backend(const google_calendar_backend& rhs) = delete;
	
	/* explicitly defaulted copy ctor */
	~google_calendar_backend() = default;

	/* lower_cooldown - implemented from the event_backend_interface */
	void lower_cooldown() override;
	/* set_cooldown - sets the normal cooldown value
	 * @seconds: cooldown in seconds
	 */
	void set_cooldown(unsigned long seconds);
	/* set_error_cooldown - sets the error cooldown value
	 * @seconds: error cooldown in seconds
	 */
	void set_error_cooldown(unsigned long seconds);
	/* set_id - sets the calendar id
	 * @id: calendar id
	 *
	 * Set the id for the Google Calendar to use.
	 */
	void set_id(const std::string& id);
	/* set_key - sets the API key
	 * @key: API key
	 *
	 * Sets the Google API key that can be used to access the Calendar.
	 */
	void set_key(const std::string& key);
	/* update - implemented from the event_backend_interface */
	std::optional<std::list<event>> update() override;

	/* cooldown - get the cooldown value
	 *
	 * Returns boost::posix_time::time_duration representation of the
	 * cooldown.
	 */
	time_duration cooldown() const;
	/* error_cooldown - ge the error cooldown value
	 *
	 * Returns boost::posix_time::time_duration representation of the 
	 * error cooldown value.
	 */
	time_duration error_cooldown() const;
	/* id - get the calendar id
	 *
	 * Returns a string view to the calendar id
	 */
	std::string_view id() const;
	/* key - get the API key
	 *
	 * Returns a string view to the API key
	 */
	std::string_view key() const;
	/* ready - implemented from the event_backend_interface */
	bool ready() const override;
};
}
