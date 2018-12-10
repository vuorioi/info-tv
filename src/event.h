#pragma once

#include <string>
#include <string_view>

#include <boost/date_time.hpp>

using boost::posix_time::ptime;
using boost::posix_time::time_period;

namespace events {
/* event class
 * This class represents an event instance
 */
class event {
public:
	/* event - ctor
	 * @name: name of the event
	 * @start_time: event start time in boost::posix_time::ptime format
	 * @end_time: event end time in boost::posix_time::ptime format
	 *
	 * Throws std::logic_error if start_time > end_time
	 */
	event(const std::wstring& name,
	      const ptime& start_time,
	      const ptime& end_time);
	/* event - explicitly defaulted copy ctor */
	event(const event& rhs) = default;

	/* ~event - explicitly defaulted dtor */
	~event() = default;

	/* set_duration - set the event duration
	 * @start: boost::posix_time::ptime representation of the start time
	 * @end: boost::posix_time::ptime representation of the end time
	 *
	 * Sets the event to span from start to end. Throws std::logica_error if
	 * start > end
	 */
	void set_duration(const ptime& start, const ptime& end);
	/* set_id - set event id
	 * @id: event id
	 *
	 * Sets the id of this event
	 */
	void set_id(const std::string& id);
	/* set_location - set the event location
	 * @location: location string
	 *
	 * Sets the event location to be the supplied string
	 */
	void set_location(const std::wstring& location);
	/* set_name - set the event name
	 * @name: name string
	 *
	 * Sets the event name to be the supplied string
	 */
	void set_name(const std::wstring& name);

	/* duration -  get the event duration
	 *
	 * Returns a boost::posix_time::time_period object representing the span
	 * of this event
	 */
	time_period duration() const;
	/* id - get the event id
	 *
	 * Returns a string view of the event id
	 */
	std::string_view id() const;
	/* location - get the event location
	 *
	 * Returns a string view of the event duration
	 */
	std::wstring_view location() const;
	/* name - get the event name
	 *
	 * Return a string view of the event name
	 */
	std::wstring_view name() const;

protected:
	time_period duration_;
	std::string id_;
	std::wstring location_;
	std::wstring name_;
};
}
