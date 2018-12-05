#pragma once

#include <algorithm>
#include <list>
#include <map>
#include <string>
#include <vector>
#include <utility>

namespace icalendar {
struct node {
	node() = default;

	std::string
	operator[](const std::string property_name)
	{
		auto it = std::find_if(properties.begin(),
				       properties.end(),
				       [&](const std::pair<std::string,
							   std::string>& p)
			{
				return p.first == property_name;
			});

		if (it == properties.end())
			return "";

		return it->second;
	}

	std::list<node>
	subnode(const std::string& name)
	{
		return subnodes.at(name);
	}

	std::string name;
	std::vector<std::pair<std::string, std::string>> properties;
	std::map<std::string, std::list<node>> subnodes;
};

//std::string clean(std::string&& str);
node parse(const std::string& src);
//node parse_node(std::stringstream& src, const std::string& node_name);
//std::pair<std::string, std::string> parse_property(const std::string& line);
//std::string trim(const std::string& str, const std::string& whitespace = " \r");
}
