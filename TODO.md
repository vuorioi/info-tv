# TODO
 - v1.1
   - [ ] Update CMake files to enable target installing
   - [ ] Write the missing unit tests
   - [ ] Rewrite `db_connection` class to allow more flexible
         formating of the request
   - [ ] Add logging
   - [ ] Remove whitespaces from before and after event descriptions
   - [ ] Shorten the cooldown period if a query or parsing fails
   - [ ] Parse cooldown from commandline
   - [ ] Event model's update should signal if there're new events since
         the last update
   - [ ] Screensaver on the empty space
   - [ ] Reimplement the event model module to use a local copy of the
         event json
   - [ ] Parsing the event name and description for English and
         Finnish versions
   - [ ] Add a label to the events showing how far away are they
         (in days or hours) or if they are happening at the moment
   - [ ] If the event description contains an url to facebook parse the
         contents of that page for description
