#include "status_view.h"

#include <algorithm>
#include <iomanip>
#include <sstream>

#include "git_commit_rev.h"

util::status_view::status_view() :
	logo_{nullptr}
{}

util::status_view::~status_view()
{
	if (logo_)
		delete logo_;
}

void
util::status_view::set_logo(const std::string& path)
{
	std::ifstream stream{path};
	if (stream.is_open())
		logo_ = new ui::ascii_image{stream};
	else
		logo_ = nullptr;
}

void
util::status_view::set_system_time(const ptime& time)
{
	system_time_ = time;
}

void
util::status_view::set_system_message(const std::wstring& msg)
{
	system_msg_ = msg;
}

void
util::status_view::draw(ui::win& win) const
{
	const unsigned logo_height = logo_ ? logo_->height : 0;
	const unsigned msg_height = system_msg_.empty() ? 0 : 3;
	const unsigned height = 3 + logo_height + msg_height;

	ui::win status_win{&win,
			   {win.curx(), win.cury()},
			   {win.curx() + win.max_width(), win.cury() + height}};

	// Draw the logo
	if (logo_)
		status_win.add_ascii_image(*logo_,
					   ui::effect::bold,
					   ui::align::center);

	// Print date and time
	std::wstringstream time_ss;
	time_ss << std::setw(2)
		<< std::setfill(L'0')
		<< system_time_.time_of_day().hours() << ":"
		<< std::setw(2)
		<< std::setfill(L'0')
		<< system_time_.time_of_day().minutes() << ":"
		<< std::setw(2)
		<< std::setfill(L'0')
		<< system_time_.time_of_day().seconds();

	std::wstringstream date_ss;
	date_ss << system_time_.date().day() << ". "
		<< system_time_.date().month() << " "
		<< system_time_.date().year();

	std::wstringstream version_ss;
	version_ss << "built from "
		   << git_commit_rev;

	status_win.add_text(date_ss.str(),
			    ui::effect::bold)
		  .add_text(time_ss.str(),
			    ui::effect::bold)
		  .add_text(version_ss.str(),
			    ui::effect::normal,
			    ui::align::right)
		  .newline();

	// Print the system message if one is present
	if (not system_msg_.empty())
		status_win.newline()
			  .add_text(system_msg_,
				    ui::effect::reverse |
				    ui::effect::blink |
				    ui::effect::bold,
				    ui::align::center);

	status_win.newline().draw();
}

unsigned
util::status_view::height() const
{
	return 0;
}

ptime
util::status_view::system_time() const
{
	return system_time_;
}

std::wstring_view
util::status_view::system_message() const
{
	return std::wstring_view(system_msg_);
}
