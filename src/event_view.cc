#include "event_view.h"

#include <chrono>

using boost::posix_time::second_clock;

/* flash_freq - flash the highlighted text at 0.5 Hz */
constexpr float flash_freq = .5;

/* flash hilight - sw flashing timer for event highlighting
 * @freq: flashing frequency
 *
 * Returns true and false periodically depending on the used frequency
 */
static bool flash_hilight(float freq);

void
events::event_view::set_model(event_model* model)
{
	model_ = model;
}

void
events::event_view::draw(ui::win& win) const
{
	// If the there are no events render a window showing an unhappy face
	if (model_->events().empty()) {
		ui::win event_win{&win,
				  {win.curx(), win.cury()},
				  {win.curx() + win.max_width(),
				  win.cury() + 5}};

		event_win.newline()
			 .add_text(L"No events   :(",
				   ui::effect::bold,
				   ui::align::center);

		return;
	}

	// Render all of the events in the model
	for (auto event : model_->events()) {
		const unsigned location_space = event.location()
						     .empty() ? 0 : 1;
		const unsigned event_space = 4 +
					     location_space;

		if (win.remaining_height() < event_space)
			break;

		if (win.remaining_height() < event_space)
			break;

		// Create a window in the parent windows curren position and
		// render all of the necessary stuff in it. If the name or the
		// description is too long they are shortened
		ui::win event_win{&win,
				  {win.curx(), win.cury()},
				  {win.curx() + win.max_width(),
				  win.cury() + event_space}};

		auto start = event.duration().begin();
		auto end = event.duration().end();
		std::wstringstream date_time_ss;

		if (start.date() == end.date())
			date_time_ss << L"On "
				     << start.date().day_of_week()
				     << ' '
				     << start.date().day()
				     << L". of "
				     << start.date().month()
				     << ' '
				     << start.date().year()
				     << L" from "
				     << std::setw(2)
				     << std::setfill(L'0')
				     << start.time_of_day().hours()
				     << ':'
				     << std::setw(2)
				     << std::setfill(L'0')
				     << start.time_of_day().minutes()
				     << L" to "
				     << std::setw(2)
				     << std::setfill(L'0')
				     << end.time_of_day().hours()
				     << ':'
				     << std::setw(2)
				     << std::setfill(L'0')
				     << end.time_of_day().minutes();
		else
			date_time_ss << L"From "
				     << start.date().day_of_week()
				     << ' '
				     << start.date().day()
				     << L". of "
				     << start.date().month()
				     << ' '
				     << start.date().year()
				     << ' '
				     << std::setw(2)
				     << std::setfill(L'0')
				     << start.time_of_day().hours()
				     << ':'
				     << std::setw(2)
				     << std::setfill(L'0')
				     << start.time_of_day().minutes()
				     << L" to "
				     << end.date().day_of_week()
				     << ' '
				     << end.date().day()
				     << L". of "
				     << end.date().month()
				     << ' '
				     << end.date().year()
				     << ' '
				     << std::setw(2)
				     << std::setfill(L'0')
				     << end.time_of_day().hours()
				     << ':'
				     << std::setw(2)
				     << std::setfill(L'0')
				     << end.time_of_day().minutes();

		std::wstringstream time_until_ss;
		auto time_until = start - second_clock::local_time();

		if (time_until.is_negative()) {
			time_until_ss << L"In progress";
		} else {
			time_until_ss << L"In ";
			
			if (time_until.hours() > 24)
				time_until_ss << time_until.hours() / 24
					      << L" d "
					      << time_until.hours() % 24
					      << L" h";
			else if (time_until.hours() > 0)
				time_until_ss << time_until.hours()
					      << L" h "
					      << time_until.minutes()
					      << L" m";
			else if (time_until.minutes() > 0)
				time_until_ss << time_until.minutes()
					      << L" m";
			else
				time_until_ss << L"a jiffy";
		}

		std::wstringstream name_ss;

		if (event.name().length() > event_win.max_width() - 1)
			name_ss << event.name()
					.substr(0, event_win.max_width() - 4)
				<< "...";
		else
			name_ss << event.name();

		if (event.hilight()) {
			event_win.add_text(name_ss.str(),
					   flash_hilight(flash_freq) ?
					   ui::effect::reverse | ui::effect::bold :
					   ui::effect::bold)
				 .newline();
		} else {
			event_win.add_text(name_ss.str(),
					   ui::effect::bold)
				 .newline();
		}

		if (not event.location().empty()) {
			 event_win.add_text(event.location().data(),
					    ui::effect::normal,
					    ui::align::left)
				  .newline();
		}

		event_win.add_text(date_time_ss.str(),
				   ui::effect::normal,
				   ui::align::append)
			 .add_text(time_until_ss.str(),
				   time_until.is_negative() ?
				   ui::effect::bold : 
				   ui::effect::normal,
				   ui::align::right)
			 .newline();

		event_win.draw();
	}
}

unsigned
events::event_view::height() const
{
	return 0;
}

static bool flash_hilight(float freq)
{
	static bool use_bold = true;
	static std::chrono::time_point<std::chrono::steady_clock> last_change;

	if (freq < 0)
		throw std::invalid_argument{"frequency cannot be negative"};

	auto now = std::chrono::steady_clock::now();

	if (now - last_change >=
	    std::chrono::milliseconds(static_cast<unsigned>(1000/(2*freq)))) {
		use_bold = !use_bold;
		last_change = now;
	}

	return use_bold;
}
