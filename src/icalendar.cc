#include "icalendar.h"

#include <sstream>
#include <stdexcept>

/* clean - remove abnormalities in strings
 * @clean: string to clean
 *
 * Returns the modified string via std::move
 */
static std::string clean(std::string&& str);
/* parse_node - parse icalendar node
 * @src: stringstream containing the icalendar data
 * @node_name: name of the node to parse
 *
 * Returns a node that can contain properties and subnodes
 *
 * This function is called recursively for all nodes found from the source
 * stringstream
 */
static icalendar::node parse_node(std::stringstream& src,
				  const std::string& node_name);
/* parse_property - parse a property name and value from string
 * @line: string containing name na value pair
 *
 * Returns a pair containing the property name and pair
 */
static std::pair<std::string, std::string> parse_property(const std::string& line);
/* trim - remove whitespaces from a string
 * @str: string to trim
 * @whitespace: string containing the characers to remove
 */
static std::string trim(const std::string& str,
			const std::string& whitespace = " \r");

icalendar::node
icalendar::parse(const std::string& src)
{
	std::stringstream src_ss{src};

	while (not src_ss.eof()) {
		std::string line;
		std::getline(src_ss, line);

		const auto pos = line.find("BEGIN:");

		if (pos != std::string::npos) {
			std::string root_name = trim(line.substr(line.find(':') + 1));

			if (root_name != "VCALENDAR")
				throw std::runtime_error{"root node is not VCALENDAR"};

			return parse_node(src_ss, root_name);
		}
	}

	throw std::runtime_error{"failed to find a root node"};
}

static std::string
clean(std::string&& str)
{
	// Replace the "\," with just ","
	for (auto i = str.find("\\,"); i != std::string::npos; i = str.find("\\,"))
		str.replace(i, 2, ",");

	return std::move(str);
}

static icalendar::node
parse_node(std::stringstream& src, const std::string& node_name)
{
	icalendar::node n;
	
	n.name = node_name;

	std::stringstream end_ss;
	end_ss << "END:" << n.name;

	while (not src.eof()) {
		std::string line;
		std::getline(src, line);

		auto pos = line.find(end_ss.str());
		if (pos != std::string::npos)
			break;

		pos = line.find("BEGIN:");
		if (pos != std::string::npos) {
			const std::string subnode_name = trim(line.substr(line.find(':') + 1));

			n.subnodes[subnode_name].push_back(parse_node(src, subnode_name));
			continue;
		}

		n.properties.emplace_back(parse_property(line));
	}

	return n;
}

static std::pair<std::string, std::string>
parse_property(const std::string& line)
{
		const auto pos = line.find(':');
		if (pos != std::string::npos) {
			const std::string name = trim(line.substr(0, pos));
			const std::string value = clean(trim(line.substr(pos + 1)));

			return std::make_pair(name, value);
		}
		
		throw std::runtime_error{"failed to parse property"};
}

static std::string
trim(const std::string& str, const std::string& whitespace)
{
	const auto begin = str.find_first_not_of(whitespace);

	if (begin == std::string::npos)
		return "";

	const auto end = str.find_last_not_of(whitespace);
	const auto len = end - begin + 1;

	return str.substr(begin, len);
}
