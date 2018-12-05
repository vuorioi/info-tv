#include "icalendar.h"

#include <sstream>
#include <stdexcept>

static std::string clean(std::string&& str);
static icalendar::node parse_node(std::stringstream& src,
				  const std::string& node_name);
static std::pair<std::string, std::string> parse_property(const std::string& line);
static std::string trim(const std::string& str,
			const std::string& whitespace = " \r");

icalendar::node
icalendar::parse(const std::string& src)
{
	std::stringstream src_ss{src};

	if (not src_ss.eof()) {
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
