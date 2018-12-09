#include <chrono>
#include <clocale>
#include <csignal>
#include <list>
#include <thread>
#include <tuple>
#include <memory>
#include <utility>
#include <vector>

#include "event_backend_interface.h"
#include "event_model.h"
#include "event_view.h"
#include "google_calendar_backend.h"
#include "parser.h"
#include "pop_calendar_backend.h"
#include "status_view.h"
#include "ui.h"
#include "utility.h"
#include "view_interface.h"

constexpr char version[] = "1.1";

static bool quit = false;

static void refresh_system_message(util::status_view& view);
static void set_system_message(util::status_view& view,
			       const std::wstring& msg,
			       std::chrono::seconds timeout);
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

	// Create views and add them to the controllers list
	std::list<view_interface*> views;
	
	util::status_view status;
	views.push_back(&status);

	events::event_view calendar_view;
	views.push_back(&calendar_view);

	// Create the event model and register the backends parsed from commandline to it
	std::vector<std::pair<std::string, std::string>> params;

	try {
		params = util::parse_commandline(argc, argv);
	} catch (const std::exception& e) {
		std::cout << "Failed to parse arguments\n\n";
		print_help(argv[0]);
		return -1;
	}

	events::event_model calendar_model;
	std::list<std::shared_ptr<events::event_backend_interface>> backends;

	for (auto param : params) {
		if (param.first == "help") {
			print_help(argv[0]);
			return 0;
		} else if (param.first == "version") {
			print_version();
			return 0;
		}

		if (param.first == "google-api") {
			auto parsed = util::parse_value<4>(param.second);
			auto backend = std::make_shared<events::google_calendar_backend>();

			backend->set_id(parsed[0]);
			backend->set_key(parsed[1]);
			backend->set_cooldown(std::stoi(parsed[2]));
			backend->set_error_cooldown(std::stoi(parsed[3]));

			backends.emplace_back(std::make_shared<events::google_calendar_backend>());
			calendar_model.add_source(backend);
		} else if (param.first == "pop-api") {
			auto parsed = util::parse_value<3>(param.second);
			auto backend = std::make_shared<events::pop_calendar_backend>();

			backend->set_url(parsed[0]);
			backend->set_cooldown(std::stoi(parsed[1]));
			backend->set_error_cooldown(std::stoi(parsed[2]));

			backends.push_back(std::move(backend));
			calendar_model.add_source(backends.back());
		} else if (param.first == "logo") {
			status.set_logo(param.second);
		} else {
			std::cout << "Unknown option '"
				  << param.first
				  << "'.\n\n";
			print_help(argv[0]);
			return -1;
		}
	}

	calendar_view.set_model(&calendar_model);

	// Setup curses and signal handler
	ui::screen_init();

	std::signal(SIGINT, signal_handler);

	unsigned screen_width, screen_height;
	std::tie(screen_width, screen_height) = ui::screen_size();

	do {
		auto start = std::chrono::steady_clock::now();

		ui::clear();

		bool new_events = calendar_model.update();
		if (new_events)
			set_system_message(status, L"New events!", 60s);

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

static void print_help(const char* name)
{
	std::cout << "Usage:\n"
		  << "  " << name << " [ --help | --version ]\n"
		  << "  " << name << " [options]\n\n"
		  << "  <id> is the Google Calendar id and <key> is the API key that can be used\n"
		  << "  to access the Calendar.\n\n"
		  << "Options:\n"
		  << "  --pop-api <url>,<cd>,<ecd>\n"
		  << "                         Add a POP backend with the <url> pointing to the ics\n"
		  << "                         resource. <cd> is the cooldown period in seconds and\n"
		  << "                         <ecd> is the cooldown period used if the connection\n"
		  << "                         to server failed.\n"
		  << "  --google-api <id>,<key>,<cd>,<ecd>\n"
		  << "                         Add a Google backend with the calendar id <id> and API\n"
		  << "                         key <key>. <cd> is the cooldown period in seconds\n"
		  << "                         and <ecd> is the cooldown period used if the connection\n"
		  << "                         to server failed.\n";
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
