#ifndef XMLHELPER_H
#define XMLHELPER_H

#include <pugixml/pugixml.hpp>

// Some additional functions to work with xml objects
namespace xmlHelper {

	// Convert the content of an xml node to an int
	int xToInt(const pugi::xml_node& n);

	// Convert the attribute p of an xml node to an int
	int xAtribToInt(const pugi::xml_node& n, const char* p);
} // namespace xmlHelper

#endif
