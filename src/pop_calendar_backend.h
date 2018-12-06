#pragma once

#include <list>
#include <optional>
#include <string>
#include <string_view>

#include <boost/date_time.hpp>

#include "db_connection.h"
#include "event.h"
#include "event_backend_interface.h"

using boost::posix_time::ptime;
using boost::posix_time::time_duration;

namespace events {
class pop_calendar_backend : public event_backend_interface {
public:
	pop_calendar_backend();
	pop_calendar_backend(const pop_calendar_backend& rhs) = delete;
	
	virtual ~pop_calendar_backend() = default;

	virtual void lower_cooldown() override;
	virtual void set_cooldown(unsigned long seconds);
	virtual void set_error_cooldown(unsigned long seconds);
	virtual void set_url(const std::string& url);
	virtual std::optional<std::list<event>> update() override;

	virtual time_duration cooldown() const;
	virtual time_duration error_cooldown() const;
	virtual bool ready() const override;
	virtual std::string_view url() const;

protected:
	time_duration cooldown_;
	db_connection db_;
	time_duration error_cooldown_;
	ptime last_update_;
	std::string url_;
	bool was_error_;
};
}
