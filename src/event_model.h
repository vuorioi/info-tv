#pragma once

#include <list>
#include <memory>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

#include "event.h"
#include "event_backend_interface.h"

namespace events {
/* event_model class
 * This class provides data for an event_view. It can get events from multiple
 * backends and combine them into a single list.
 */
class event_model {
public:
	event_model() = default;
	event_model(const event_model& rhs) = delete;
	~event_model() = default;

	/* add_hilight - add a rule for hilighting events
	 * @source: index of the source to highlight
	 *
	 * This overload highlight all events provided by the event source of the
	 * given index. If the index is out of bounds this function has no effect.
	 */
	void add_hilight(unsigned source);
	/* add_hilight - add a rule for hilighting events
	 * @rule: regex used for finding the events to highlight
	 * @target: the part of the event this rule will target
	 *
	 * This overload adds a regex rule that is used to match every event.
	 */
	void add_hilight(std::basic_regex<wchar_t> rule,
			 events::search_target target);
	/* add_source - add an event source to this model
	 * @source: shared pointer to the event backend
	 *
	 * Adds an source that this model will use for finding events.
	 */
	void add_source(std::shared_ptr<event_backend_interface> source);
	/* update - try to update the model
	 *
	 * Returns true if there are new events.
	 *
	 * This function updates the model removing past events and getting new
	 * ones from the event backends registered to this model.
	 */
	bool update();

	/* events - return the list of events
	 *
	 * Returns a list containing the events in this model.
	 */
	std::list<event> events() const;
	
protected:
	std::list<event> events_;
	mutable std::list<std::pair<std::shared_ptr<event_backend_interface>,
			  std::list<event>>> event_sources_;
	std::vector<std::pair<events::search_target,
			      std::basic_regex<wchar_t>>> regex_rules_;
	std::unordered_set<unsigned> source_rules_;
};
}
