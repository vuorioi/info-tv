#include "parser.h"

#include <codecvt>
#include <cstring>
#include <locale>

#include <nlohmann/json.hpp>

#include "event.h"
#include "icalendar.h"

/* parse_datetime - parse date and time from the json provided by the Google
 * API
 * @src: string containing the date and time
 */
static ptime parse_datetime(const std::string& src)
{
	auto temp{src};

	temp.replace(temp.find_first_of('T'), 1, 1, ' ');
	temp.erase(temp.find_first_of('+'));
	
	try {
		return time_from_string(temp);
	} catch (...) {
		throw std::runtime_error{"failed to parse datetime"};
	}
}

std::pair<std::string, std::string>
events::parser::credentials_from_args(const int count, char** vector)
{
	std::string key;
	std::string id;

	for (unsigned i = 0; i < count; i++) {
		const char* arg = vector[i];

		if (std::strcmp(arg, "--calendar_id") == 0)
			id = vector[++i];
		else if (std::strcmp(arg, "--api_key") == 0)
			key = vector[++i];
		else
			continue;
	}

	return std::pair{id, key};
}

std::list<events::event>
events_from_ics(const std::string& ics_str)
{
	using boost::posix_time::from_iso_string;

	std::list<events::event> event_list;
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

	icalendar::node root = icalendar::parse(ics_str);

	if (root["VERSION"] != "2.0")
		throw std::runtime_error{"root node has wrong icalendar version"};

	if (root["PRODID"] != "TUT.FI//POP-CALENDARSERVICE_V1.0//FI")
		throw std::runtime_error{"wrong product id"};

	for (auto e : root.subnode("VEVENT")) {
		if (e["STATUS"] != "CONFIRMED")
			continue;

		event_list.emplace_back(events::event{converter.from_bytes(e["SUMMARY"].c_str()),
					from_iso_string(e["DTSTART;TZID=Europe/Helsinki"]),
					from_iso_string(e["DTEND;TZID=Europe/Helsinki"])});
		const std::string location = e["LOCATION"];

		if (not location.empty())
			event_list.back().set_location(converter.from_bytes(location.c_str()));
	}

	return event_list;
}

std::list<events::event>
events::parser::events_from_json(const std::string& json_str)
{
	using json = nlohmann::json;

	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

	json data;

	try {
		data = json::parse(json_str);
	} catch (...) {
		throw std::runtime_error{"failed to parse the event json"};
	}

	auto kind_it = data.find("kind");
	if (kind_it == data.end())
		throw std::runtime_error{"can't find key \"kind\" from header"};

	if (*kind_it != "calendar#events")
		throw std::runtime_error{"header: key \"kind\" has an unsupported value"};
	
	auto item_it = data.find("items");
	if (item_it != data.end()) {
		auto events = *item_it;
		std::list<event> event_list;

		for (auto event : events) {
			std::string name;
			std::string location;
			ptime start;
			ptime end;

			auto iter = event.find("kind");
			if (iter == event.end())
				throw std::runtime_error{"can't find key \"kind\" from event"};
			if (*iter != "calendar#event")
				throw std::runtime_error{"event: key \"kind\" has and unsupported value"};

			iter = event.find("status");
			if (iter == event.end())
				throw std::runtime_error{"can't find key \"status\" from event"};
			if (*iter == "tentative") {
				continue;
			} else if (*iter == "confirmed") {
				iter = event.find("summary");
				if (iter == event.end())
					throw std::runtime_error{"can't find key \"summary\" from event"};
				else
					name = *iter;

				iter = event.find("location");
				if (iter != event.end())
					location = *iter;

				iter = event.find("start");
				if (iter == event.end())
					throw std::runtime_error{"can' find key \"start\" from event"};

				auto start_time = *iter;

				iter = start_time.find("dateTime");
				if (iter == start_time.end()) {
					iter = start_time.find("date");
					if (iter == start_time.end()) {
						throw std::runtime_error{"can't find \"dateTime\" or \"date\" from event start time"};
					} else {
						try {
							start = ptime{from_string(*iter)};
						} catch (...) {
							throw std::runtime_error{"failed to parse start date"};
						}
					}
				} else {
					start = parse_datetime(*iter);
				}

				iter = event.find("end");
				if (iter == event.end())
					throw std::runtime_error{"can't find key \"end\" from event"};

				auto end_time = *iter;

				iter = end_time.find("dateTime");
				if (iter == end_time.end()) {
					iter = end_time.find("date");
					if (iter == end_time.end()) {
						throw std::runtime_error{"can't find \"dateTime\" or \"date\" from event end time"};
					} else {
						try {
							end = ptime{from_string(*iter)};
						} catch (...) {
							throw std::runtime_error{"failed to parse end date"};
						}
						
					}
				} else {
					end = parse_datetime(*iter);
				}

				event_list.emplace_back(events::event{converter.from_bytes(name.c_str()), start, end});

				if (not location.empty())
					event_list.back().set_location(converter.from_bytes(location.c_str()));
			} else {
				throw std::runtime_error{"event key \"status\" has an unkown value"};
			}
		}

		return event_list;
	} else {
		return {};
	}
}
