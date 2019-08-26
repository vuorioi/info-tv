#include "motd_view.h"

#include <algorithm>


namespace motd {
void
motd_view::set_model(motd::motd_model* model)
{
	model_ = model;
}

void
motd_view::draw(ui::win& win) const
{
    if (model_->empty())
        return;

    if (model_->changed())
        cached_msg_ = model_->message();

    auto window_width = win.max_width();
    std::wstring motd_text(window_width, ' ');

    if (cached_msg_.length() > window_width) {
        //TODO scroll the text from right to left
        win.add_text(cached_msg_, ui::effect::bold)
           .newline();
    } else if (cached_msg_.length() < window_width) {
        static unsigned window_pos = 0;
        static bool pos_inc = true;

        motd_text.replace(window_pos,
                          cached_msg_.length(),
                          cached_msg_);

        win.add_text(motd_text, ui::effect::bold)
           .newline();

        if (pos_inc) {
            if (++window_pos == window_width - cached_msg_.length())
                pos_inc = false;
        } else {
            if (--window_pos == 0)
                pos_inc = true;
        }
    } else {
        win.add_text(cached_msg_, ui::effect::bold)
           .newline();
    }
}

unsigned
motd_view::height() const
{
	return model_->empty() ? 0 : 1;
}
} // namespace motd
