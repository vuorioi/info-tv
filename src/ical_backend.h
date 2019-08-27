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
/* ical_backend class
 * This class provides an event backend to the POP calendar used by Tampere
 * University of Technology.
 */
class ical_backend : public event_backend_interface {
protected:
	time_duration cooldown_;
	time_duration error_cooldown_;
    std::string host_;
    http_get http_client_;
	ptime last_update_;
	std::string target_;
	bool was_error_;

public:
	/* ctor
     * @ctx: io_context for the http_client
	 *
	 * This ctor creates a POP event calendar backend object with cooldown
	 * period of 1 hour and an error cooldown of 10 minutes.
	 */
	ical_backend(boost::asio::io_context& ctx);
	ical_backend(const ical_backend& rhs) = delete;
	
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
	/* set_url - set the url components
	 * @host: host to connect to
     * @target: target resource path on host
	 *
	 * Sets the url where the calendar data is requested from
	 */
	void set_url(std::string host, std::string target);
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
	/* ready - implemented from the event_backend_interface */
	bool ready() const override;
	/* url - get the url
	 *
	 * Returns a pair of string views to the host and target resoucer used by this
     * backend as the source
	 */
    std::pair<std::string_view, std::string_view> url() const;
};
}
