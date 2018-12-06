#include "event_model.h"

#include <algorithm>
#include <iterator>
#include <optional>
#include <sstream>

#include <boost/date_time.hpp>

using boost::posix_time::second_clock;
//using boost::gregorian;

static bool add_events(std::list<events::event>& dst,
		       const std::list<events::event>& src);

void
events::event_model::add_source(event_backend_interface* source)
{
	event_sources_.push_back(source);
}

bool
events::event_model::update()
{
	bool new_events = false;

	for (auto source : event_sources_) {
		// If source cooldown has expired update events
		// and on success add events to model's event list.
		// on a failure the sources cooldown is lowered to
		// schedule a retry sooner
		if (source->ready()) {
			auto result = source->update();

			if (result.has_value())
				(add_events(events_, result.value()) or
				 new_events) ? new_events = true :
					       new_events = false;
			else
				source->lower_cooldown();
		}
	}
	
	// Remove events that have already ended
	events_.remove_if([](const events::event& e) {
		return e.duration().end() < second_clock::local_time();
	});

	return new_events;
}

std::list<events::event>
events::event_model::events() const
{
	return events_;
}

/* event_comparison structure
 * This structure represents a comparison between one to multiple events.
 * It can be used to see if the lhs event, stored inside the struct, is equal
 * to any other event or if any other event start after the lhs event.
 */
struct event_comparison {
	/* event_comparison - ctor */
	event_comparison(const events::event& lhs) :
		lhs_{lhs},
		is_same_{false}
	{};

	/* was_equal - check if the compared rhs event was equal to lsh event
	 *
	 * Returns true if the last compared rhs event was (probably) the same
	 * event as the lhs event.
	 */
	bool
	was_equal() const
	{
		return is_same_;
	}
	/* operator() - compares the lhs and rhs events
	 * @rhs: the event to compare to the lhs event
	 *
	 * Returns true if the events are (probably) the same or
	 * if the rhs event starts after the lhs event.
	 *
	 * Events are considered to be the same if their names are close to
	 * each other and if they have the same duration and if their locations
	 * are close to each other.
	 */
	bool
	operator()(const events::event& rhs)
	{
		if (are_close(lhs_.name(), rhs.name()) and
		    lhs_.duration() == rhs.duration() and
		    are_close(lhs_.location(), rhs.location())) {
			is_same_ = true;
			return true;
		} else {
			is_same_ = false;
			return (lhs_.duration().begin() <
				rhs.duration().begin());
		}
	}

private:
	/* are_close - check if strings are "close" to each other
	 * @lhs: first wide string to compare
	 * @rhs: second wide string to compare
	 *
	 * Returns true if either one of the string is a substring of the
	 * other.
	 */
	static bool
	are_close(const std::wstring_view& lhs, const std::wstring_view& rhs)
	{
		return (lhs.find(rhs) != std::string::npos or
			rhs.find(lhs) != std::string::npos);
	}

	const events::event& lhs_;
	bool is_same_;
};

static bool
add_events(std::list<events::event>& dst, const std::list<events::event>& src)
{
	bool new_events = false;

	for (auto new_event : src) {
		// Find the first existing event that starts after the new
		// event or that is (probably) the same event
		event_comparison cmp(new_event);

		auto it = find_if(dst.begin(), dst.end(), cmp);

		// If the event was equal to an existing event we can skip it
		// otherwise it is added before the first event that starts afte
		// it. If no existing event start after this event then it is
		// inserted to the end of the list
		if (cmp.was_equal()) {
			continue;
		} else {
			dst.insert(it, new_event);
			new_events = true;
		}
	}

	return new_events;
}
