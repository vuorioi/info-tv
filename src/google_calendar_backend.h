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
class google_calendar_backend : public event_backend_interface {
public:
	google_calendar_backend();
	google_calendar_backend(const google_calendar_backend& rhs) = delete;
	
	virtual ~google_calendar_backend() = default;

	virtual void lower_cooldown() override;
	virtual void set_cooldown(unsigned long seconds);
	virtual void set_error_cooldown(unsigned long seconds);
	virtual void set_id(const std::string& id);
	virtual void set_key(const std::string& key);
	virtual std::optional<std::list<event>> update() override;

	virtual time_duration cooldown() const;
	virtual time_duration error_cooldown() const;
	virtual std::string_view id() const;
	virtual std::string_view key() const;
	virtual bool ready() const override;

protected:
	time_duration cooldown_;
	db_connection db_;
	time_duration error_cooldown_;
	std::string id_;
	std::string key_;
	ptime last_update_;
	bool was_error_;
};
}
