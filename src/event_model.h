#pragma once

#include <list>
#include <memory>
#include <string>

#include "event.h"
#include "event_backend_interface.h"

namespace events {
/* event_model class
 * This class provides data for an event_view. It gets events from a Google
 * Calendar with REST API calls and parses the resulting json.
 */
class event_model {
public:
	/* event_model - explicitly defaulted ctor */
	event_model() = default;
	/* event_model - explicitly deleted copy ctor */
	event_model(const event_model& rhs) = delete;
	
	/* ~event_model - explicitly defaulted dtor */
	virtual ~event_model() = default;

	/* add_source - add an event source to this model
	 * @source: shared pointer to the event backend
	 *
	 * Adds an source that this model will use for finding events.
	 */
	virtual void add_source(std::shared_ptr<event_backend_interface> source);
	/* update - try to update the model
	 *
	 * Returns true if there are new events.
	 *
	 * This function updates the model removing past events and getting new
	 * ones from the event backends registered to this model.
	 */
	virtual bool update();

	/* events - return the list of events
	 *
	 * Returns a list containing the events in this model.
	 */
	virtual std::list<event> events() const;
	
protected:
	std::list<event> events_;
	std::list<std::shared_ptr<event_backend_interface>> event_sources_;
};
}
