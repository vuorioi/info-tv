#include <chrono>
#include <clocale>
#include <csignal>
#include <list>
#include <thread>
#include <tuple>

#include "event_model.h"
#include "event_view.h"
#include "parser.h"
#include "status_view.h"
#include "ui.h"
#include "view_interface.h"

constexpr char version[] = "1.0";

static bool quit = false;

static void refresh_system_message(util::status_view& view);
static void set_system_message(util::status_view& view,
			      const std::wstring& msg,
			      std::chrono::seconds timeout);
static bool parse_help(const int count, char** vector);
static bool parse_version(const int count, char** vector);
static void print_help(const char* name);
static void print_version();
static void signal_handler(int signo);

struct system_messages {
	std::chrono::seconds timeout;
	std::chrono::time_point<std::chrono::steady_clock> set_time;
} system_messages;


int main(int argc, char** argv)
{
	using namespace std::chrono_literals;

	if (parse_help(argc, argv)) {
		print_help(argv[0]);
		return 0;
	}

	if (parse_version(argc, argv)) {
		print_version();
		return 0;
	}

	std::string calendar_id;
	std::string api_key;
	std::tie(calendar_id, api_key) = events::parser::credentials_from_args(argc, argv);

	std::list<view_interface*> views;
	
	util::status_view status;
	views.push_back(&status);

	events::event_view calendar_view;
	events::event_model calendar_model{calendar_id, api_key, 600};
	views.push_back(&calendar_view);
	calendar_view.set_model(&calendar_model);

	bool calendar = false;
	if (calendar_id.empty() or api_key.empty())
		set_system_message(status, L"No calendar id or key set!", 300s);
	else
		calendar = true;

	// Setup curses and signal handler
	ui::screen_init();

	std::signal(SIGINT, signal_handler);

	unsigned screen_width, screen_height;
	std::tie(screen_width, screen_height) = ui::screen_size();

	do {
		auto start = std::chrono::steady_clock::now();

		ui::clear();

		if (calendar) {
			bool success = calendar_model.update();
			if (not success)
				set_system_message(status,
						   L"Failed to get events from Google API!",
						   120s);
		}

		status.set_system_time(second_clock::local_time());
		refresh_system_message(status);

		// Draw everything
		ui::win main_win{{2, 1},
				 {screen_width - 2, screen_height - 1}};
	
		for (auto view : views)
			view->draw(main_win);

		main_win.draw();

		auto end = std::chrono::steady_clock::now();
		auto loop_duration = end - start;

		std::this_thread::sleep_for(std::chrono::milliseconds(100) - loop_duration);
	} while (not quit);
	
	// Clean up curses
	ui::screen_deinit();

	return 0;
}

static void refresh_system_message(util::status_view& view)
{
	auto now = std::chrono::steady_clock::now();
	auto duration = now - system_messages.set_time;

	if (duration >= system_messages.timeout)
		view.set_system_message(L"");
}

static void set_system_message(util::status_view& view,
			      const std::wstring& msg,
			      std::chrono::seconds timeout)
{
	view.set_system_message(msg);
	system_messages.timeout = timeout;
	system_messages.set_time = std::chrono::steady_clock::now();
}

static bool parse_help(const int count, char** vector)
{
	for (unsigned i = 0; i < count; i++) {
		const char* arg = vector[i];

		if (std::strcmp(arg, "--help") == 0)
			return true;
	}

	return false;
}

static bool parse_version(const int count, char** vector)
{
	for (unsigned i = 0; i < count; i++) {
		const char* arg = vector[i];

		if (std::strcmp(arg, "--version") == 0)
			return true;
	}

	return false;
}

static void print_help(const char* name)
{
	std::cout << "Usage:\n"
		  << "  " << name << " --help | --version\n"
		  << "  " << name << " --calendar_id <id> --api_key <key> [options]\n\n"
		  << "  <id> is the Google Calendar id and <key> is the API key that can be used\n"
		  << "  to access the Calendar.\n\n"
		  << "Options:\n"
		  << "  --cooldown             Time delay between event refreshes in seconds.\n"
		  << "                         Default value is 600 and the value has to be\n"
		  << "                         greater than 60. Not yet implemented!\n";
}

static void print_version()
{
	std::cout << "info-tv version "
		  << version
		  << '\n';
}

static void signal_handler(int signo)
{
	if (signo == SIGINT)
		quit = true;
}
