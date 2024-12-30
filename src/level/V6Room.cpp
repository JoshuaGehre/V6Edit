#include "V6Room.h"
#include <string.h>

#include "../other/xmlHelper.h"

// Names of the xml attributes for the different properties
static const char* v6_RoomPropertyNames[V6_ROOMDATA_COUNT] = {
	"tileset",
	"tilecol",
	"platx1",
	"platy1",
	"platx2",
	"platy2",
	"platv",
	"enemyx1",
	"enemyy1",
	"enemyx2",
	"enemyy2",
	"enemytype",
	"directmode",
	"warpdir",
};

void V6Room::addEntity(V6Entity& Ent)
{
	entities.push_back(Ent);
	V6Entity& E = entities[entities.size() - 1];
	E.x() %= V6_ROOM_X;
	E.y() %= V6_ROOM_Y;
}

void V6Room::setData(const pugi::xml_node& N)
{
	roomName = N.child_value();
	for(int i = 0; i < V6_ROOMDATA_COUNT; i++) {
		data[i] = xmlHelper::xAtribToInt(N, v6_RoomPropertyNames[i]);
	}
}

void V6Room::simplePrint(std::ostream& Out)
{
	for(int y = 0; y < V6_ROOM_Y; y++) {
		for(int x = 0; x < V6_ROOM_X; x++) {
			char symb = '?';
			int id = block(x, y);
			if(tileset() == 0) {
				// Space station tileset (tiles.png)
				if(id == 0) {
					symb = ' ';
				} else if((id == 1) || ((id >= 80) && (id <= 679)) || (id == 59)) {
					symb = '#';
				} else if(((id >= 6) && (id <= 9)) || ((id >= 49) && (id <= 50))) {
					symb = 'x';
				} else {
					symb = ':';
				}
			} else {
				// Any other tileset (tiles2.png)
				if(id == 0) {
					symb = ' ';
				} else if((id == 1) || ((id >= 80) && (id <= 679)) || (id == 740)) {
					symb = '#';
				} else if(((id >= 6) && (id <= 9)) || ((id >= 49) && (id <= 79))) {
					symb = 'x';
				} else {
					symb = ':';
				}
			}
			Out << symb;
		}
		Out << "\n";
	}
}

void V6Room::printEntities(int rx, int ry, std::ostream& Out)
{
	for(V6Entity& Ent : entities) {
		Ent.print(rx, ry, Out);
	}
}

void V6Room::printMetaData(std::ostream& Out)
{
	Out << "<edLevelClass";
	for(int i = 0; i < V6_ROOMDATA_COUNT; i++) {
		Out << ' ' << v6_RoomPropertyNames[i] << "=\"" << data[i] << '"';
	}
	Out << '>' << roomName << "</edLevelClass>\n";
}
