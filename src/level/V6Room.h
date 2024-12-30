#ifndef V6ROOM_H
#define V6ROOM_H

#include <iostream>
#include <vector>

#include "../V6Properties.h"
#include "V6Entity.h"

#define V6_ROOMDATA_COUNT 14

// A room contains the blocks,
// roomMetaData and entities
class V6Room {
  private:
	uint16_t blocks[V6_ROOM_XY]{};

	std::vector<V6Entity> entities{};

	int data[V6_ROOMDATA_COUNT] =
		{0, 0, 0, 0, 320, 240, 4, 0, 0, 320, 240, 0, 0, 0};

	std::string roomName{};

  public:
	// Add an entity to this room
	// This will change the coordinates of
	// the entity relative to this room
	void addEntity(V6Entity& Ent);

	// Read room MetaData from its xml node
	void setData(const pugi::xml_node& N);

	// Print a very simple view of the room
	// ' ' = Air
	// '#' = Solid
	// 'x' = Spike
	// ':' = Background Decoartion (No Collision)
	void simplePrint(std::ostream& Out);

	// Access to the blocks
	inline uint16_t& block(int x, int y) { return blocks[x + V6_ROOM_X * y]; };

	// Access to the important parts of the data
	// Maybe add platform and entity bounds here
	inline int& tileset() { return data[0]; };
	inline int& tilecol() { return data[1]; };
	inline int& directmode() { return data[12]; };
	inline int& warpdir() { return data[13]; };

	void printEntities(int rx, int ry, std::ostream& Out);

	void printMetaData(std::ostream& Out);

	inline const std::string& getName() const { return roomName; };

	inline std::vector<V6Entity>& getEntities() { return entities; };
};

#endif // V6ROOM_H
