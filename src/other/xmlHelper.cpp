#include "xmlHelper.h"
#include <string>

int xmlHelper::xToInt(const pugi::xml_node& n)
{
	return std::stoi(n.child_value());
}

int xmlHelper::xAtribToInt(const pugi::xml_node& n, const char* p)
{
	return std::stoi(n.attribute(p).value());
}
