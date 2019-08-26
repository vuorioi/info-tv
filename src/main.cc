#include <chrono>
#include <clocale>
#include <codecvt>
#include <csignal>
#include <list>
#include <regex>
#include <thread>
#include <tuple>
#include <memory>
#include <utility>
#include <vector>

#include <boost/asio.hpp>

#include "event.h"
#include "event_backend_interface.h"
#include "event_model.h"
#include "event_view.h"
#include "google_calendar_backend.h"
#include "ical_backend.h"
#include "motd_model.h"
#include "motd_view.h"
#include "parser.h"
#include "status_view.h"
#include "ui.h"
#include "utility.h"
#include "view_interface.h"


extern char version[];

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


int main(int argc, const char** argv)
{
	using namespace std::chrono_literals;

    boost::asio::io_context ctx;

	// Create views and add them to the controllers list
	std::list<view_interface*> views;
	
	util::status_view status;
	views.push_back(&status);

	motd::motd_view motd_view;
	views.push_back(&motd_view);

    ///TEST
    motd::motd_model motd_model(ctx, 18, "skilta-pw");
	motd_view.set_model(&motd_model);
    ///TEST

	events::event_view calendar_view;
	views.push_back(&calendar_view);

	// Create the event model and register the backends parsed from commandline to it
	std::vector<std::pair<std::string, std::vector<std::string>>> params;

	try {
		params = util::parse_commandline(argc, argv);
	} catch (const std::exception& e) {
		std::cout << "Failed to parse arguments\n\n";
		print_help(argv[0]);
		return -1;
	}

	events::event_model calendar_model;
	std::list<std::shared_ptr<events::event_backend_interface>> backends;

	for (const auto [name, values] : params) {
		if (name == "help") {
			print_help(argv[0]);
			return 0;
		} else if (name == "version") {
			print_version();
			return 0;
		}

		if (name == "gcal-api") {
			auto option_count = values.size();

			if (option_count == 2) {
				auto backend = std::make_shared<events::google_calendar_backend>();

				backend->set_id(values[0]);
				backend->set_key(values[1]);

				backends.emplace_back(std::move(backend));
			} else if (option_count == 4) {
				auto backend = std::make_shared<events::google_calendar_backend>();

				backend->set_id(values[0]);
				backend->set_key(values[1]);

				try {
					backend->set_cooldown(std::stoi(values[2]));
					backend->set_error_cooldown(std::stoi(values[3]));
				} catch (const std::exception& e) {
					std::cout << "Invalid cooldown argument for --gcal-api\n\n";
					print_help(argv[0]);
					return -1;
				}

				backends.emplace_back(std::move(backend));
			} else {
				std::cout << "Wrong amount of arguments for --gcal-api\n\n";
				print_help(argv[0]);
				return -1;
			}

			calendar_model.add_event_source(backends.back());
		} else if (name == "ical-api") {
			auto option_count = values.size();

			if (option_count == 1) {
				auto backend = std::make_shared<events::ical_backend>();

				backend->set_url(values[0]);

				backends.push_back(std::move(backend));
			} else if (option_count == 3) {
				auto backend = std::make_shared<events::ical_backend>();

				backend->set_url(values[0]);

				try {
					backend->set_cooldown(std::stoi(values[1]));
					backend->set_error_cooldown(std::stoi(values[2]));
				} catch (const std::exception& e) {
					std::cout << "Invalid cooldown argument for --ical-api\n\n";
					print_help(argv[0]);
					return -1;
				}

				backends.push_back(std::move(backend));
			} else {
				std::cout << "Wrong amount of arguments for --ical-api\n\n";
				print_help(argv[0]);
				return -1;
			}

			calendar_model.add_event_source(backends.back());
		} else if (name == "logo") {
			if (values.size() == 0) {
				status.set_logo("/usr/local/share/info-tv/logo.ascii");
			} else if (values.size() == 1) {
				status.set_logo(values[0]);
			} else {
				std::cout << "Wrong amount of arguments for --logo\n\n";
				print_help(argv[0]);
				return -1;
			}
		} else if (name == "hilight") {
			if (values.size() == 1) {
				int index;

				try {
					index = std::stoi(values[0]);
				} catch (const std::exception& e) {
					std::cout << "Invalid argument for --hilight: "
						  << values[0]
						  << "\n\n";
					print_help(argv[0]);
					return -1;
				}

				if (index < 0) {
					std::cout << "Invalid argument for --hilight: "
						  << values[0]
						  << "\n\n";
					print_help(argv[0]);
					return -1;
				}

				calendar_model.add_hilight(index);
			} else if (values.size() == 3) {
				if (values[0] == "search") {
					std::basic_regex<wchar_t> r;

					std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
					std::wstring regex_expr = converter.from_bytes(values[2]);

					try {
						r = std::basic_regex<wchar_t>(regex_expr,
									      //std::regex_constants::egrep |
									      std::regex_constants::icase |
									      std::regex_constants::optimize);
					} catch (const std::regex_error& e) {
						std::cout << "Regex parsing failed! "
							  << e.what()
							  << " For: "
							  << values[2]
							  << "\n\n";
						print_help(argv[0]);
						return -1;
					}

					unsigned target = 0;

					if (values[1] == "name") {
						target |= events::search_target::name;
					} else if (values[1] == "description") {
						target |= events::search_target::description;
					} else if (values[1] == "location") {
						target |= events::search_target::location;
					} else if (values[1] == "all") {
						target |= events::search_target::name |
							  events::search_target::description |
							  events::search_target::location;
					} else {
						std::cout << "Unknown target for --hilight search: "
							  << values[1]
							  << "!\n\n";
						print_help(argv[0]);
						return -1;
					}

					calendar_model.add_hilight(std::move(r), static_cast<events::search_target>(target));
				} else {
					std::cout << "Unknown argument for --hilight: "
						  << values[0]
						  << "\n\n";
					print_help(argv[0]);
					return -1;
				}
			} else {
				std::cout << "Wrong amount of arguments for --highlight\n\n";
				print_help(argv[0]);
				return -1;
			}
		} else {
			std::cout << "Unknown option '"
				  << name
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

    auto worker = std::thread([&ctx] {
            ctx.run();
        });

	do {
		auto start = std::chrono::steady_clock::now();

		ui::clear();

		// Update the system message
		bool new_events = calendar_model.update();
		if (new_events)
			set_system_message(status, L"Events updated!", 60s);

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

    ctx.stop();

    worker.join();
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
		  << "  " << name << " [ options ]\n\n"
		  << "Options:\n"
		  << "  --ical-api <url> [ <cd> <ecd> ]\n"
		  << "                         Add an ical backend with the <url> pointing to the ics\n"
		  << "                         resource. <cd> is the cooldown period in seconds and\n"
		  << "                         <ecd> is the cooldown period used if the connection\n"
		  << "                         to server failed.\n"
		  << "  --gcal-api <id> <key> [ <cd> <ecd> ]\n"
		  << "                         Add a Google backend with the calendar id <id> and API\n"
		  << "                         key <key>. <cd> is the cooldown period in seconds\n"
		  << "                         and <ecd> is the cooldown period used if the connection\n"
		  << "                         to server failed.\n"
		  << "  --hilight <source> | search <target> <regex>\n"
		  << "                         Highlight events that are either from the source number\n"
		  << "                         <source> (indexing starts from 0) or that match the\n"
		  << "                         <regex> in <target>. <target> can be any one of these:\n"
		  << "                         'name', 'description', 'location' or 'all'.\n"
		  << "  --logo <path>          Path to a text file containing the ascii graphic logo\n"
		  << "                         to display at the top of the screen.\n";
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
