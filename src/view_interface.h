#pragma once

#include "ui.h"

/* view_interface class
 * This abstract class provides an uniform interface for views
 * that can be attached to main window
 */
class view_interface {
public:
	/* draw - draw the view of the provided window
	 * @win: parent window
	 *
	 * This function is called by the parent of the view to render
	 * its content on inside the window provided
	 */
	virtual void draw(ui::win& win) const = 0;
	/* height - returns the height of this view
	 * 
	 * Returns the height needed by this view to fully render its
	 * content
	 */
	virtual unsigned height() const = 0;
};
