#ifndef V6ENTITY_H
#define V6ENTITY_H

#include <string>

#include <pugixml/pugixml.hpp>

#define V6_ENTITYDATA_COUNT 9

// An entity containing its data
// x and y coordinates may be stored
// relative to the room it is in
class V6Entity {
  public:
	// Store entity data in an array
	int data[V6_ENTITYDATA_COUNT];

	// Script name in case of a
	// terminal or scriptbox
	std::string str;

	// Create an entity from its xml node
	V6Entity(const pugi::xml_node& N);
	// Copy constructor
	V6Entity(const V6Entity& E) = default;
	~V6Entity() = default;

	// Access to the entity data by name
	inline int& x() { return data[0]; };
	inline int& y() { return data[1]; };
	inline int& t() { return data[2]; };
	inline int& p(int i) { return data[2 + i]; };
	inline int& p1() { return data[3]; };
	inline int& p2() { return data[4]; };
	inline int& p3() { return data[5]; };
	inline int& p4() { return data[6]; };
	inline int& p5() { return data[7]; };
	inline int& p6() { return data[8]; };

	void print(int rx, int ry, std::ostream& Out);
};

#endif
