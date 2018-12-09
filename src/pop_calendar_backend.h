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
/* pop_calendar_backend class
 * This class provides an event backend to the POP calendar used by Tampere
 * University of Technology.
 */
class pop_calendar_backend : public event_backend_interface {
public:
	/* ctor
	 *
	 * This ctor creates a POP event calendar backend object with cooldown
	 * period of 1 hour and an error cooldown of 10 minutes.
	 */
	pop_calendar_backend();
	/* explicitly deleted copy ctor */
	pop_calendar_backend(const pop_calendar_backend& rhs) = delete;
	
	/* explicitly defaulted copy ctor */
	virtual ~pop_calendar_backend() = default;

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
	/* set_url - set the url used as source
	 * @url: url to use
	 *
	 * Sets the url where the calendar data is requested from
	 */
	virtual void set_url(const std::string& url);
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
	/* ready - implemented from the event_backend_interface */
	virtual bool ready() const override;
	/* url - get the url
	 *
	 * Returns a string view to the url used by this backend as the source
	 */
	virtual std::string_view url() const;

protected:
	time_duration cooldown_;
	db_connection db_;
	time_duration error_cooldown_;
	ptime last_update_;
	std::string url_;
	bool was_error_;
};
}
