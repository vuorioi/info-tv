#pragma once

#include "motd_model.h"
#include "ui.h"
#include "view_interface.h"

namespace motd {

/* motd_view - message of the day rendering module
 */
class motd_view : public view_interface {
	motd::motd_model* model_;
    mutable std::wstring cached_msg_;

public:
	/* set_model - attach an motd_model
	 * @model: pointer to an motd_model
	 *
	 * This function sets the motd model as the data source for this view
	 */
	void set_model(motd::motd_model* model);

	/* implemented from the view_interface */
	void draw(ui::win& win) const override;
	unsigned height() const override;
};

} // namespace motd
