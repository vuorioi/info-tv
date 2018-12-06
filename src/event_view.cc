#include "event_view.h"

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
		const unsigned description_space = event.description()
							.empty() ? 0 : 1;
		const unsigned event_space = 4 +
					     location_space + 
					     description_space;

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

		std::wstringstream name_ss;

		if (event.name().length() > event_win.max_width() - 1)
			name_ss << event.name()
					.substr(0, event_win.max_width() - 4)
				<< "...";
		else
			name_ss << event.name();

		event_win.add_text(name_ss.str(),
				   ui::effect::bold,
				   ui::align::left)
			 .newline();

		if (not event.location().empty()) {
			 event_win.add_text(event.location().data(),
					    ui::effect::normal,
					    ui::align::left)
				  .newline();
		}

		event_win.add_text(date_time_ss.str(),
				   ui::effect::normal,
				   ui::align::append)
			 .newline();

		if (not event.description().empty()) {
			std::wstringstream description_ss;

			description_ss << '"';

			if (event.description().length() > event_win.max_width() - 2)
				description_ss << event.description()
						       .substr(0, event_win.max_width() - 5)
					       << "...";
			else
				description_ss << event.description();
			
			description_ss << '"';
			event_win.add_text(description_ss.str(),
					   ui::effect::highlight);
		}

		event_win.draw();
	}
}

unsigned
events::event_view::height() const
{
	return 0;
}
