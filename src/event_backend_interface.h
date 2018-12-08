#pragma once

#include <list>
#include <optional>

#include "event.h"

namespace events {
/* event_backend_interface class
 * This class provides the interface that event backends need to implement
 * to work with the event_model class.
 *
 * The idea of event backends is that they implement a way to get events from
 * a remove source (server, etc.) periodically. A backend has a cooldown value
 * to control the frequency of requests made to the remote source.
 *
 * Event model should always query if the backend's cooldown has passed with
 * ready() function before calling update(). Event model can request the backend
 * to lower the cooldown time by calling lower_cooldown() function. The cooldown
 * period should be set back to normal after an successfull update() call.
 */
class event_backend_interface {
public:
	/* update - try to get new events from the backend
	 *
	 * Returns an optional containing a list of the events provided by
	 * the backend if the cooldown period has passed and the request
	 * was successfull. Otherwise the optional is empty.
	 */
	virtual std::optional<std::list<event>> update() = 0;
	/* lower_cooldown - instruct the backend to use a lower cooldown
	 *
	 * This function tells the backend to lower the value used as cooldown
	 * value. This can be used to schedule a retry after an failed update.
	 */
	virtual void lower_cooldown() = 0;

	/* ready - check if the backends cooldown is not active
	 *
	 * Returns true if a new request to the backend can be performed false
	 * otherwise
	 */
	virtual bool ready() const = 0;
};
}
