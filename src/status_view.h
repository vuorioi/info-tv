#pragma once

#include <ctime>
#include <string>
#include <string_view>

#include <boost/date_time.hpp>

#include "ui.h"
#include "view_interface.h"

using namespace boost::posix_time;
using namespace boost::gregorian;

namespace util {
/* status_view class
 * This class draws system info to the main window
 */
class status_view : public view_interface {
public:
	status_view();
	/* ~status_view - dtor
	 *
	 * This function frees  the logo if it was allocated
	 */
	~status_view();

	/* set_logo - set system logo
	 * @path: path to the logo file
	 *
	 * This function sets the logo displayed at the top of the screen
	 */
	void set_logo(const std::string& path);
	/* set_system_time - set the displayed time
	 * @time: boost::posix_time::ptime representation of time
	 *
	 * This function sets the time rendered by the view
	 */
	void set_system_time(const ptime& time);
	/* set_system_message - set the displayed message
	 * @message: message displayed
	 *
	 * This function sets the system message which is displayed with
	 * highlights
	 */
	void set_system_message(const std::wstring& msg);

	/* implemented from the view_interface */
	void draw(ui::win& win) const override;
	unsigned height() const override;
	/* system_time - return the displayed time
	 *
	 * Returns boost::posix_time::ptime representation of the rendered
	 * time
	 */
	ptime system_time() const;
	/* system_message - return the system message
	 *
	 * Returns a string view to the system message rendered
	 */
	std::wstring_view system_message() const;

protected:
	ui::ascii_image* logo_;
	std::wstring system_msg_;
	ptime system_time_;
};
}
