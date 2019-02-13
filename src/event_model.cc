#include "event_model.h"

#include <algorithm>
#include <iterator>
#include <optional>
#include <sstream>

#include <boost/date_time.hpp>

#include "utility.h"

using boost::posix_time::second_clock;

static bool add_events(std::list<events::event>& dst,
		       const std::list<events::event>& src,
	   	       const std::vector<std::pair<events::search_target, std::basic_regex<wchar_t>>>& rules);
static bool do_regex_search(const std::pair<events::search_target, std::basic_regex<wchar_t>>& rule,
			    const events::event& event);
static void combine_with(events::event& lhs, const events::event& rhs);

void
events::event_model::add_hilight(unsigned source)
{
	source_rules_.insert(source);
}

void
events::event_model::add_hilight(std::basic_regex<wchar_t> rule,
				 const events::search_target target)
{
	regex_rules_.emplace_back(std::pair(target, std::move(rule)));
}

void
events::event_model::add_event_source(std::shared_ptr<event_backend_interface> source)
{
	event_sources_.push_back(std::pair(source, std::list<event>{}));
}

void
events::event_model::add_motd_source(std::shared_ptr<event_backend_interface> source)
{
	motd_sources_.push_back(std::pair(source, std::list<std::wstring>{}));
}

bool
events::event_model::update()
{
	bool new_events = false;

	for (auto& [source, source_events] : event_sources_) {
		if (source->ready()) {
			auto result = source->update();

			if (result.has_value()) {
				new_events = true;
				source_events = result.value();
			} else {
				source->lower_cooldown();
				continue;
			}
		}
	}

	// If any of the sources provided updates we need to rebuild our local
	// events list. Otherwise we can just remove passed events
	if (new_events) {
		unsigned i = 0;
		for (auto& [source, source_events] : event_sources_) {
			if (source_rules_.find(i) != source_rules_.end())
				for (auto& event : source_events)
					event.set_hilight(true);

			add_events(events_, source_events, regex_rules_);

			i++;
		}
	} else {
		events_.remove_if([](const events::event& e) {
			return e.duration().end() <
			       second_clock::local_time();
		});
	}

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
	event_comparison(const events::event& lhs) :
		is_same_{false},
		lhs_{lhs}
	{};

	/* operator() - compares the lhs and rhs events
	 * @rhs: the event to compare to the lhs event
	 *
	 * Returns true if the events are (probably) the same or
	 * if the rhs event starts after the lhs event.
	 *
	 * Events are considered to be the same if their names are similar
	 * and their durations intersect.
	 */
	bool
	operator()(const events::event& rhs)
	{
		if (util::similar_str(lhs_.name(), rhs.name()) and
		    lhs_.duration().intersects(rhs.duration())) {
			is_same_ = true;
			return true;
		} else {
			is_same_ = false;
			return (lhs_.duration().begin() <
				rhs.duration().begin());
		}
	}

	bool
	was_equal() const
	{
		return is_same_;
	}

private:
	bool is_same_;
	const events::event& lhs_;
};

/* add_events - add new events
 * @dst: destination list
 * @src: source list
 * @regexes: a vector of regexes for event highlighting
 *
 * Returns true if new events where added otherwise false.
 *
 * This function adds new events from src to dst list and combines duplicate
 * events. Events that match any one of the regexes from the vector are
 * hilighted.
 */
static bool
add_events(std::list<events::event>& dst,
	   const std::list<events::event>& src,
	   const std::vector<std::pair<events::search_target, std::basic_regex<wchar_t>>>& rules)
{
	bool new_events = false;

	for (auto new_event : src) {
		// Find the first existing event that starts after the new
		// event or that is (probably) the same event
		event_comparison cmp(new_event);

		auto it = find_if(dst.begin(), dst.end(), std::ref(cmp));

		// If the event was equal to an existing event we can skip it
		// otherwise it is added before the first event that starts after
		// it. If no existing event start after this event then it is
		// inserted to the end of the list
		if (cmp.was_equal()) {
			combine_with(*it, new_event);
		} else {

			for (auto& rule : rules)
				if (not new_event.hilight())
					new_event.set_hilight(do_regex_search(rule, new_event));

			dst.insert(it, std::move(new_event));
			new_events = true;
		}
	}

	return new_events;
}

/* do_regex_search - use the regex rule to search the event for keywords
 * @rule: pair of regex object and a target specifing which source to use
 * @event: the event to search
 *
 * Returns true if the rule matches this event, otherwise false is returned.
 */
static bool
do_regex_search(const std::pair<events::search_target, std::basic_regex<wchar_t>>& rule,
		const events::event& event)
{
	bool found = false;

	if (rule.first & events::search_target::name) {
		found |= std::regex_search(event.name().data(), rule.second);
	}

	if (rule.first & events::search_target::description) {
		found |= std::regex_search(event.description().data(), rule.second);
	}
	
	if (rule.first & events::search_target::location) {
		found |= std::regex_search(event.location().data(), rule.second);
	}

	return found;
}

/* combine_with - combine contained event with another
 * @lsh: modified event
 * @rhs: event to combine with
 *
 * Combines the two events into one.
 */
static void
combine_with(events::event& lhs, const events::event& rhs)
{
	if (lhs.name().length() < rhs.name().length())
		lhs.set_name(rhs.name().data());

	if (lhs.location().length() < rhs.location().length())
		lhs.set_location(rhs.location().data());

	// If either one of the events is highlighted the combined event will
	// be highlighted
	lhs.set_hilight(lhs.hilight() or rhs.hilight());

	auto duration = lhs.duration().span(rhs.duration());
	auto start = duration.begin();
	auto end = duration.end();

	lhs.set_duration(start, end);
}
