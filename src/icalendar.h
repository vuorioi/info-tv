#pragma once

#include <algorithm>
#include <list>
#include <map>
#include <string>
#include <vector>
#include <utility>

namespace icalendar {
/* node struct
 * This struct represents an icalendar node.
 */
struct node {
	node() = default;

	/* operator[] - find a property
	 * @property_name: name of the property to search for
	 *
	 * Returns the value of the property or empty string.
	 */
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

	/* subnodes - get subnodes
	 * @subnode_name: name of the subnode
	 * 
	 * Returns a list of all the subnodes with the name.
	 */
	std::list<node>
	subnode(const std::string& subnode_name)
	{
		return subnodes.at(name);
	}

	std::string name;
	std::vector<std::pair<std::string, std::string>> properties;
	std::map<std::string, std::list<node>> subnodes;
};

/* parse - parse the icalendar data
 * @src: string containing icalendar data
 *
 * Returns a root node of the icalendar data
 *
 * This function parses icalendar data from the string and returns the root node
 */
node parse(const std::string& src);
}
