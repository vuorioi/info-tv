#pragma once

#include <string>

#include "event_model.h"
#include "view_interface.h"

namespace events {
/* event_view class
 * This class fulfills the view_interface requirements and is responsible for
 * rendering the events provided by an event_model
 */
class event_view : public view_interface {
public:
	/* event_view - explicitly defaulted ctor */
	event_view() = default;
	/* event_view - explicitly deleted copy ctor */
	event_view(const event_view& rhs) = delete;

	/* ~event_view - explicitly defaulted dtor */
	~event_view() = default;

	/* set_model - attach an event_model
	 * @model: pointer to an event_model
	 *
	 * This function sets the event model as the source data for this view
	 */
	void set_model(event_model* model);

	/* implemented from view_interface */
	void draw(ui::win& win) const override;
	unsigned height() const override;
	
protected:
	event_model* model_;
};
}
