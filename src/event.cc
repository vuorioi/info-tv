#include "event.h"

#include <stdexcept>

events::event::event(const std::wstring& name,
		     const ptime& start_time,
		     const ptime& end_time) :
	duration_{start_time, end_time},
	name_{name}
{
	if (start_time > end_time)
		throw std::logic_error{"start time has to be before end"};
}

void
events::event::set_duration(const ptime& start, const ptime& end)
{
	if (start > end)
		throw std::logic_error{"start time has to be before end"};

	duration_ = time_period{start, end};
}

void
events::event::set_location(const std::wstring& location)
{
	location_ = location;
}

void
events::event::set_name(const std::wstring& name)
{
	name_ = name;
}

time_period
events::event::duration() const
{
	return duration_;
}

std::wstring_view
events::event::location() const
{
	return std::wstring_view{location_};
}

std::wstring_view
events::event::name() const
{
	return std::wstring_view{name_};
}

