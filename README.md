# Info TV
This project implements a commandline based information TV display
for a Raspberry Pi. Both Google Calendar and iCalendar source can
be used as the information sources. Rendering is done (at the moment)
with ncursesw and connections to the remote services are handled with
libcurl using the interfaces provided by TUT and Google.

Google Calendar API provides the event information in a json format
and this is parsed with the help of nlohmann's json library. iCalendar
format is parsed using a a simple purpose built parser. Boost libraries
are used for unit testing and date time utilites.

The interfaces provided by the components are documented in the
source files.

## Project structure
The directory structure of this repo is the following
 - `media/` -- Media files
 - `src/` -- Source code
   - `db_connection` -- Handles the connections to the remote services
   - `event` -- Represents a single event
   - `event_model` -- A model that manages the events in the system
   - `view_interface` -- A commong interface for the view that are
     shown to the user
   - `event_view` -- A view for rendering events from the event model
   - `status_view` -- A view for system information
   - `event_backend_interface` -- A common interface for the event
     sources
   - `google_calendar_backend` -- Implementation of the event backend
     interface for a Google calendar
   - `pop_calendar_backend` -- Implementation of the event backend
     interface for a POP calendar
   - `icalendar` -- Very simple parser for the iCalendar format
   - `parser` -- Event parsing functions
   - `ui` -- Simple userinterface based on the ncursesw library
   - `utility` -- Collection of utility functions
 - `test/` -- Unit test sources
 - `util/` -- Utility scripts
 - `CMakeLists.txt`
 - `COPYING.md` -- License information
 - `README.md` -- This file
 - `TODO.md` -- List of things to be done until the next release

## Dependencies
This project uses c++17 features and as such a relatively recent
compiler should be used. This project alse depends on the
following libraries (versions are the ones I have successfully
linked against but older versions might work just as well):
 - Boost unit testing framework 1.68.0
 - Boost date time 1.68.0
 - libcurl 7.62.0
 - ncurses 6.1
 - nlohmann/json 3.4.0

## How to build and use
### Building and installing
After installing the required dependencies and cloning this repo
the software can be build and installed using the following
commands:

```
mkdir build
cd build
cmake ..
make
sudo make install
```

### Running
After building and installing, the software can be run with
```
info-tv [ options | --help | --version ]
```
Options allow connecting to multiple event sources, event highlighting
and more.

To add a Google Calendar backend the following option is used:
```
--google-api <id> <key> [ <cd> <ecd> ]
```
Set `<id>` to the id of the Google Calendar to use and `<key>`
to the API key connected to that Calendar. Optionally `<cd>` and
`<ecd>` can be provided to manually se the cooldown and error
cooldown values.

To add a POP Calendar backend the following option is used:
```
--pop-api <url> [ <cd> <ecd> ]
```
Set `<url>` to the url provided by the TUT intra. Optionally `<cd>`
and `<ecd>` can be provided to manually se the cooldown and error
cooldown values.
 
To display a graphics at the top of the status view provide the
following option:
```
--logo [<path>]
```
If `<path>` is not set, the default logo will be used. To use you own
logo set `<path>` to the relative (from the working directory) or
absolute path to the text file containing an ascii image. For example
`--logo /path/to/your/logo.ascii`

To highlight events the `--hilight` option can be used in two ways:
```
--hilight <event_source>
```
Where event source is the number of the source (indexing starts from
0) of the events. This will highlight all events from the specified
source.

```
--hilight search <target> <regex>
```
This option will search using the inputed `regex` from any one of the
following targets:
 - `name`: search the name of the event
 - `description`: search the event description
 - `location`: search the event location
 - `all`: carry out the search on all of the targets

Regex uses the modified ECMAScript regular expression grammar (for
more info please see the C++ standard etc.).

Do note that backslashes have to be escaped. E.g. to use the following
regular expression `\b\w{4}\s\w+` (search for two words [the first
word is exactly four characters long] separated by a whitespace) it
should be transformed to `--hilight search <target> "\\b\\w{4}\\s\\w+"`.

For the most recent options and help run the program using the `--help`
option.
