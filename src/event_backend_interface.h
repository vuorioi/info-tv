#pragma once

#include <list>
#include <optional>

#include "event.h"

namespace events {
class event_backend_interface {
public:
	virtual std::optional<std::list<event>> update() = 0;
	virtual void lower_cooldown() = 0;

	virtual bool ready() const = 0;
};
}
