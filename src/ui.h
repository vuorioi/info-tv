#pragma once

#include <algorithm>
#include <fstream>
#include <string>
#include <utility>
#include <vector>

#define NCURSES_NOMACROS
#include <curses.h>

namespace ui {
/* enum effect
 * Describes the effect of the rendered text. More than one effect can be
 * selected at a time.
 */
enum effect { blink = A_BLINK,
	      bold = A_BOLD,
	      highlight = A_ITALIC,
	      reverse = A_REVERSE,
	      underline = A_UNDERLINE,
	      normal = A_NORMAL };

/* enum aling
 * Describes the aligment of the rendered text. Only one should be selected at a
 * time.
 *
 * @append: add text after the last text added
 * @left: add text to the left edge of the window
 * @center: add text center of the window
 * @right: add text to the right edge of the window
 */
enum align { append,
	     left,
	     center,
	     right };

/* clear - clear the screen */
void clear();
/* screen_deinit - deinit the curses
 *
 * This function frees the resources used by curses
 */
void screen_deinit();
/* screen_init - init curses
 *
 * This function inits curses library and sets it up for drawing
 */
void screen_init();
/* screen_size - return the screen size
 *
 * Returns a pair containing the width and height of the screen
 */
std::pair<unsigned, unsigned> screen_size();

/* ascii_image struct
 * This struct holds the data of an ascii image
 */
struct ascii_image {
	/* ascii_image - ctor
	 * @stream: stream containing the image data
	 *
	 * This ctor reads the image data from the stream
	 */
	ascii_image(std::ifstream& stream)
	{
		unsigned i = 0;
		width = 0;

		while (not stream.eof()) {
			std::string line;
			std::getline(stream, line);

			width = std::max(width, unsigned(line.length()));

			data.emplace_back(std::move(line));
		}

		height = data.size();
	}

	std::vector<std::string> data;
	unsigned height;
	unsigned width;
};

/* win class
 * This class represents a window which can drawn inside
 */
class win {
public:
	/* win - ctor
	 *
	 * This ctor creates a new main window (a window without a parent) 
	 * between from and to x,y-coordinates
	 */
	win(std::pair<unsigned, unsigned> from, std::pair<unsigned, unsigned> to);
	/* win - ctor
	 *
	 * This ctor creates a new subwindwow (inside the parent window) between
	 * from and to x,y-coordinates. The coordinates are from the top left
	 * corner of the parent window
	 */
	win(win* parent,
	    std::pair<unsigned, unsigned> from,
	    std::pair<unsigned, unsigned> to);

	/* ~win - dtor
	 * 
	 * This function frees the curses window data
	 */
	~win();

	/* newline - adds a newline
	 *
	 * Returns reference to a win object
	 *
	 * This function shifts the cursor down one line
	 */
	win& newline();
	/* newline - add a number of newline
	 * @lines: the number of lines to shift
	 *
	 * Returns reference to a win object
	 *
	 * This function shifts the cursor down a number of lines
	 */
	win& newline(unsigned lines);
	/* add_text - add a text to the window
	 * @text: text to add
	 * @type: effect to use in rendering, default to normal
	 * @pos: position of the text, default to appending after current cursor
	 * 	 positions
	 *
	 * Returns reference to a win object
	 *
	 * This function renders text to the window using the effect and position
	 * defined
	 */
	win& add_text(const std::wstring& text,
		      const unsigned type = normal,
		      const unsigned pos = append);
	/* add_ascii_image - add image to the window
	 * @img: reference to the image to add
	 * @type: effect to use in rendering, defaults to normal
	 * @pos: position of the image, default to appending after current cursor
	 * 	 position
	 * 
	 * Returns reference to a win object
	 *
	 * This function renders an image to the window using the effect and
	 * position defined
	 */
	win& add_ascii_image(const ascii_image& img,
			     const unsigned type = normal,
			     const unsigned pos = append);

	/* draw - draw this window
	 *
	 * This function should be called when all of the contents is added
	 * to render the window
	 */
	void draw() const;
	/* get_win - get the underlaying ncurses window
	 *
	 * Returns a pointer to the underlaying ncurses window structure
	 */
	WINDOW* get_win() const;
	/* curx - get the cursor x position
	 *
	 * Returns the current cursor x position inside the window. Value 0 means
	 * the first column starting from left
	 */
	unsigned curx() const;
	/* cury - get the cursor y position
	 *
	 * Returns the current cursor y position inside the window. Value 0 means
	 * the first line starting from the top
	 */
	unsigned cury() const;
	/* max_width - get the maximum width of item
	 *
	 * Returns the maximum width of an item inside this window
	 */
	unsigned max_width() const;
	/* remaining_height - get the remaining vertical space
	 * 
	 * Returns the remaining vertical space in this window
	 */
	unsigned remaining_height() const;

private:
	static constexpr unsigned x_padding = 2;
	static constexpr unsigned y_padding = 1;

	unsigned height_;
	win* parent_;
	unsigned width_;
	WINDOW* win_;
	unsigned x_;
	unsigned y_;
};
}
