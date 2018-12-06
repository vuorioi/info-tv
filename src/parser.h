#pragma once

#include <list>
#include <utility>
#include <string>

#include "event.h"

namespace events::parser {
	/* events_from_ics - parse events from ics data
	 * @ics_str: a string containing the icalendar data
	 *
	 * Returns a list containing the events parsed from the icalendar data.
	 * Throws std::runtime_error or std::out_of_range if the parsing fails in
	 * any way.
	 */
	std::list<event> events_from_ics(const std::string& ics_str);

	/* events_from_json - parse events from json data
	 * @json_str: a string containing the json data
	 *
	 * Returns a list containing the events parsed from the json. Throws
	 * std::runtime_error if the parsing fails in any way.
	 */
	std::list<event> events_from_json(const std::string& json_str);
}
