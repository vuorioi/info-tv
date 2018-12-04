# Info TV
This project implements an information TV display for a Raspberry
Pi using a Google Calendar as the information source. The rendering is
done (at the moment) with ncurses and the connection to the Google API is
handled with libcurl using the Google REST API.

Google Calendar API provides the event information in a json format and this
is parsed with the help of nlohmann's json library. Boost libraries are used
for testing and date time utilites.

The interfaces provided by the components are documented in the source files.

## Project structure
The file structure of this repo is the following
 - `media/` -- This directory contains the media files used in this project
 - `src/` -- This directory contains the source files for this project
 - `test/` -- This directory contains the testing source files
 - `util/` -- This directory contains utility scripts
 - `CMakeLists.txt` -- top CMake file
 - `COPYING.md` -- license information
 - `README.md` -- this file
 - `TODO.md` -- list of things to be done in future

## Dependencies
This project depends on the following libraries:
 - Boost unit testing framework 1.68.0
 - Boost date time 1.68.0
 - libcurl 7.62.0
 - ncurses 6.1
 - nlohmann/json 3.4.0

## How to build and use
### Building
After installing the required dependecies and cloning this repo
the software can be build using the following commands:

```
mkdir build
cd build
cmake ..
make
```

### Running
After building, the software can be run with

```
./src/info-tv --calendar_id <id> --api_key <key>
```

Where the `<id>` is the id of the Google Calendar from where the
events are to be pulled from and `<key>` is the API key for
accessing the calendar. Please follow the tutorials provided by
Google to get the id and API key.

For more options and help use the `--help` option.
