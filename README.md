# Info TV
This project implements an information TV display for a Raspberry
Pi using a Google Calendar and TUT's POP Calendar as the information sources.
Rendering is done (at the moment) with ncursesw and the connection to the
APIs is handled with libcurl using the interfaces provided by TUT and Google.

Google Calendar API provides the event information in a json format and this
is parsed with the help of nlohmann's json library. TUT's Calendar is provided
in iCalendar format and a purpose built parser is used for this. Boost libraries
are used for unit testing and date time utilites.

The interfaces provided by the components are documented in the source files.

## Project structure
The directory structure of this repo is the following
 - `media/` -- Media files
 - `src/` -- Source code
 - `test/` -- Unit test sources
 - `util/` -- Utility scripts
 - `CMakeLists.txt`
 - `COPYING.md` -- License information
 - `README.md` -- This file
 - `TODO.md` -- List of things to be done until the next release

## Dependencies
This project uses c++17 features an as such a relatively recent
compiler should be used. This project alse depends on the
following libraries (versions are the ones I have successfully
linked against but older versions might work just as well):
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
./src/info-tv [ options ]
```
Options allow connecting to multiple event sources.

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
following:
```
--logo <path>
```
Set `<path>` to the relative (from the working directory) or
absolute path to the text file containing the ascii image.
For example `--logo ../media/logo.ascii`

To highlight events the `highlight` optiona can be used in two ways:
```
--hilight <event_source>
```
Where event source is the number of the source (indexing starts from
0) of the events. This will highlight all events from the specified
source.

```
--hilight search <target> <regex>
```
This option will search using the inputed regex from any one of the
following targets: 'name', 'description', 'location' or 'all'.
Regex uses the modified ECMAScript regular expression grammar (for
more info please see the C++ standard etc.). Do note that backslashes
have to be escaped. E.g. to use the following regular expression
`\b\w{4}\s\w+` (search for two words [the first word is exactly four
characters long] separated by a whitespace) it should be transformed
to `\\b\\w{4}\\s\\w+`.

For the most recent options and help use the `--help` option.
