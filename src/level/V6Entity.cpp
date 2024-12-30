#include "V6Entity.h"

#include <iostream>

#include "../V6Properties.h"
#include "../other/xmlHelper.h"

// Names of the xml attributes for the different properties
static const char* v6_EntityPropertyNames[V6_ENTITYDATA_COUNT] = {
	"x",
	"y",
	"t",
	"p1",
	"p2",
	"p3",
	"p4",
	"p5",
	"p6",
};

V6Entity::V6Entity(const pugi::xml_node& N)
{
	// V6 does weird line breaks during these tags so just use the normal characters
	str = "";
	const char* p = N.child_value();
	while(*p >= 32) {
		str += *p;
		p++;
	}
	for(int i = 0; i < V6_ENTITYDATA_COUNT; i++) {
		data[i] = xmlHelper::xAtribToInt(N, v6_EntityPropertyNames[i]);
	}
}

void V6Entity::print(int rx, int ry, std::ostream& Out)
{
	Out << "<edentity";
	x() += rx * V6_ROOM_X;
	y() += ry * V6_ROOM_Y;
	for(int i = 0; i < V6_ENTITYDATA_COUNT; i++) {
		Out << ' ' << v6_EntityPropertyNames[i] << "=\"" << data[i] << '"';
	}
	x() -= rx * V6_ROOM_X;
	y() -= ry * V6_ROOM_Y;
	Out << '>' << str << "</edentity>\n";
}
