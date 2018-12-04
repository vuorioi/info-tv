#include "ui.h"

#include <array>
#include <cstdlib>

/* print_wchar_t - print a wide char string
 * @win: ncurses window to print on
 * @x: x position
 * @y: y position
 * @unicode_str: unicode string (wide string) to print
 *
 * This helper function prints the unicode string in the ncurses window win
 * in the coordinates provided. Should the conversion fail a std::runtime_error
 * is thrown
 */
static void print_wchar_t(WINDOW* win,
			  const int x,
			  const int y,
			  const wchar_t* unicode_str)
{
	int local_x = x;

	for (wchar_t* c = const_cast<wchar_t*>(unicode_str); *c != '\0'; c++) {
		int ret;
		std::array<char, sizeof(wchar_t) + 1> temp;
		temp.fill('\0');

		ret = std::wctomb(&temp[0], *c);
		if (ret < 0)
			throw std::runtime_error{"conversion from wchar_t to char failed"};
		
		if (ret > 0) {
			mvwprintw(win, y, local_x, &temp[0]);
			local_x++;
		}
	}
}

void
ui::clear()
{
	erase();
}

void
ui::screen_deinit()
{
	endwin();
}

void
ui::screen_init()
{
	setlocale(LC_ALL, "");
	initscr();
	// Do not capture special key combinations
	nocbreak();
	// Do not echo inputs
	noecho();
	// Do not render cursor
	curs_set(0);
}

std::pair<unsigned, unsigned>
ui::screen_size()
{
	unsigned width, height;

	getmaxyx(stdscr, height, width);

	return std::pair{width, height};
}

ui::win::win(std::pair<unsigned, unsigned> from,
	     std::pair<unsigned, unsigned> to) :
	height_{std::get<1>(to) - std::get<1>(from)},
	parent_{nullptr},
	width_{std::get<0>(to) - std::get<0>(from)},
	x_{x_padding},
	y_{y_padding}
{
	win_ = newwin(height_, width_,
		      std::get<1>(from), std::get<0>(from));
	box(win_, 0, 0);
}

ui::win::win(win* parent,
	     std::pair<unsigned, unsigned> from,
	     std::pair<unsigned, unsigned> to) :
	height_{std::get<1>(to) - std::get<1>(from)},
	parent_{parent},
	width_{std::get<0>(to) - std::get<0>(from)},
	x_{x_padding},
	y_{y_padding}
{
	parent->newline(height_);

	win_ = derwin(parent_->get_win(), height_, width_,
		      std::get<1>(from), std::get<0>(from));
	box(win_, 0, 0);
}

ui::win::~win()
{
	delwin(win_);
}

ui::win&
ui::win::newline()
{
	y_++;
	x_ = x_padding;

	return *this;
}

ui::win&
ui::win::newline(unsigned lines)
{
	y_ += lines;
	x_ = x_padding;

	return *this;
}

ui::win&
ui::win::add_text(const std::wstring& text,
		  const unsigned type,
		  const unsigned pos)
{
	// Preload the x coordinates based on the value of pos
	switch (pos) {
	case left:
		x_ = x_padding;
		break;

	case right:
		x_ = width_ - x_padding - text.length();
		break;

	case center:
		x_ = width_/2 - text.length()/2;
		break;

	case append:
	default:
		;// Nop
	}

	unsigned remaining_space = width_ - x_padding - x_;

	wattron(win_, type);

	if (text.length() > remaining_space)
		print_wchar_t(win_,
			      x_, y_,
			      text.substr(0, remaining_space).c_str());
		//mvwprintw(win_,
			  //y_, x_,
			  //text.substr(0, remaining_space).c_str());
	else
		print_wchar_t(win_, x_, y_, text.c_str());
		//mvwprintw(win_, y_, x_, text.c_str());

	x_ += std::min(unsigned(text.length()) + 1, remaining_space);

	wattroff(win_, type);

	return *this;
}

ui::win&
ui::win::add_ascii_image(const ascii_image& img,
			 const unsigned type,
			 const unsigned pos)
{
	// Preload the x coordinates based on the value of pos
	switch (pos) {
	case left:
		x_ = x_padding;
		break;

	case right:
		x_ = width_ - x_padding - img.width;
		break;

	case center:
		x_ = width_/2 - img.width/2;
		break;

	case append:
	default:
		;// Nop
	}

	wattron(win_, type);

	for (auto line : img.data) {
		mvwprintw(win_, y_, x_, line.c_str());
		y_++;
	}

	wattroff(win_, type);

	x_ = x_padding;

	return *this;
}

WINDOW*
ui::win::get_win() const
{
	return win_;
}

unsigned 
ui::win::curx() const
{
	return x_;
}

unsigned
ui::win::cury() const
{
	return y_;
}

unsigned 
ui::win::max_width() const
{
	return width_ - 2*x_padding;
}

void 
ui::win::draw() const
{
	if (parent_)
		touchwin(parent_->get_win());

	wrefresh(win_);
}
