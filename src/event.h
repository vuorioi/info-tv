#pragma once

#include <string>
#include <string_view>

#include <boost/date_time.hpp>

using namespace boost::posix_time;
using namespace boost::gregorian;

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
	virtual ~event() = default;

	/* set_duration - set the event duration
	 * @start: boost::posix_time::ptime representation of the start time
	 * @end: boost::posix_time::ptime representation of the end time
	 *
	 * Sets the event to span from start to end. Throws std::logica_error if
	 * start > end
	 */
	virtual void set_duration(const ptime& start, const ptime& end);
	/* set_location - set the event location
	 * @location: location string
	 *
	 * Sets the event location to be the supplied string
	 */
	virtual void set_location(const std::wstring& location);
	/* set_name - set the event name
	 * @name: name string
	 *
	 * Sets the event name to be the supplied string
	 */
	virtual void set_name(const std::wstring& name);

	/* duration -  get the event duration
	 *
	 * Returns a boost::posix_time::time_period object representing the span
	 * of this event
	 */
	virtual time_period duration() const;
	/* location - get the event location
	 *
	 * Returns a string view of the event duration
	 */
	virtual std::wstring_view location() const;
	/* name - get the event name
	 *
	 * Return a string view of the event name
	 */
	virtual std::wstring_view name() const;

protected:
	time_period duration_;
	std::wstring location_;
	std::wstring name_;
};
}
