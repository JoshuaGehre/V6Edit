#include "V6Level.h"

#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <string>

#include "../other/xmlHelper.h"

#include "ContentParser.h"

// Just leave escapes like &apos; within strings
// This isn't an editor for writing levelnotes
const unsigned int parseOptions = pugi::parse_default & ~pugi::parse_escapes;

V6Level::V6Level(std::string fileName)
{
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(fileName.c_str(), parseOptions);
	(void) result;
	// V6 Puts these two around all data for no apparent reason
	pugi::xml_node data = doc.child("MapData").child("Data");

	// Load MetaData
	metaData.levelMusic = xmlHelper::xToInt(data.child("levmusic"));
	pugi::xml_node mData = data.child("MetaData");
	for(int i = 0; i < V6_METADATA_COUNT; i++) {
		metaData.data[i] = mData.child(v6_MetaDataNodeNames[i]).child_value();
	}

	// Get the size of the level
	int sX = xmlHelper::xToInt(data.child("mapwidth"));
	int sY = xmlHelper::xToInt(data.child("mapheight"));
	if((sX < 1) || (sY < 1) || (sX > V6_ROOMS_X) || (sY > V6_ROOMS_Y)) {
		throw std::invalid_argument("Level size outside of valid range!");
	}
	sizeX = sX;
	sizeY = sY;

	// Construct all rooms
	rooms.resize(sizeX * sizeY);

	// Read all blocks
	ContentParser contPar(data.child("contents").child_value());
	for(unsigned int yr = 0; yr < sizeY; yr++) {
		for(unsigned int yb = 0; yb < V6_ROOM_Y; yb++) {
			for(unsigned int xr = 0; xr < sizeX; xr++) {
				for(unsigned int xb = 0; xb < V6_ROOM_X; xb++) {
					room(xr, yr).block(xb, yb) = contPar.getNextId();
				}
			}
		}
	}

	// Ved stores its additional data in an entity
	// This contains flag names and level notes
	std::string vedData = "";

	// Read all entities
	pugi::xml_node entities = data.child("edEntities");
	for(pugi::xml_node i : entities.children("edentity")) {
		V6Entity Ent(i);
		int rx = Ent.x() / V6_ROOM_X;
		int ry = Ent.y() / V6_ROOM_Y;
		if((rx >= 0) && (ry >= 0) && (rx < (int) sizeX) && (ry < (int) sizeY)) {
			room(rx, ry).addEntity(Ent);
			//std::cout << Ent.x() << " " << Ent.y() << " " << Ent.str << "\n";
		} else if(((Ent.x() == VED_ENTITY_X) && (Ent.y() == VED_ENTITY_Y)) || ((Ent.x() == VED_ENTITY_X_ALT) && (Ent.y() == VED_ENTITY_Y_ALT))) {
			// Entity storing data used by Ved
			vedData = Ent.str;
		} else {
			std::cout << "Ignoring out of bounds entity!\n";
			std::cout << "X: " << Ent.x() << "\tY: " << Ent.y() << "\n";
		}
	}
	//std::cout << vedData << "\n";

	// Read room data
	pugi::xml_node roomData = data.child("levelMetaData");
	int j = 0;
	for(pugi::xml_node i : roomData.children("edLevelClass")) {
		// V6 stores the room data for 20x20 rooms
		// even if the level is smaller
		// Ignore data for rooms out of bounds
		int rx = j % V6_ROOMS_X;
		int ry = j / V6_ROOMS_X;
		if((rx >= 0) && (ry >= 0) && (rx < (int) sizeX) && (ry < (int) sizeY)) {
			// This room actually exists
			room(rx, ry).setData(i);
		}
		j++;
	}

	// If existent split the ved meta data
	// these parts are seperated by '|'
	if(vedData.size() > 0) {
		const char* p = vedData.c_str();
		const char* p0 = p;
		// Read the parts of ved meta data
		while(*p != '\0') {
			p++;
			if((*p == '|') || (*p == '\0')) {
				vedMetaData.push_back(std::string(p0, p - p0));
				p0 = p + 1;
			}
		}
	}
	// If ved meta data does not exist add it or add parts of it
	switch(vedMetaData.size()) {
		case 0:
			// ved version number
			vedMetaData.push_back("3");
		case 1:
			// flag names
			vedMetaData.push_back(
				"$$$$$$$$$$"
				"$$$$$$$$$$"
				"$$$$$$$$$$"
				"$$$$$$$$$$"
				"$$$$$$$$$$"
				"$$$$$$$$$$"
				"$$$$$$$$$$"
				"$$$$$$$$$$"
				"$$$$$$$$$$"
				"$$$$$$$$$"
				// A total of 99 '$' seperating 100 unnamed flags
			);
		case 2:
			// Unused / Reserved
			vedMetaData.push_back("");
		case 3:
			// Could be used by ved plugins
			vedMetaData.push_back("");
		case 4:
			// Level notes
			vedMetaData.push_back("");
	}
	// Read the ved version number
	{
		const char* p = vedMetaData[0].c_str();
		vedVersionNumber = 0;
		while(*p != '\0') {
			vedVersionNumber = 10 * vedVersionNumber + *p - '0';
			p++;
		}
		// This was tested around ved version number 3
		if(vedVersionNumber != 3) {
			std::cout << "The loaded level uses an unknown ved version number!\n"
						 "This tool was only tested for a ved version number of 3";
		}
	}
	// Read flag names
	{
		const char* p = vedMetaData[1].c_str();
		const char* p0 = p;
		int num = 0;
		if(*p == '$') {
			flagNames[0] = "";
			num++;
			p0++;
		}
		while(*p != '\0') {
			p++;
			if((*p == '$') || (*p == '\0')) {
				if(num >= 100) throw std::out_of_range(
					"Ved meta data contains more the 100 flag names!");
				flagNames[num] = std::string(p0, p - p0);
				num++;
				p0 = p + 1;
			}
		}
		//if(num < 100) std::cout << "Less than 100 flag names listed!";
	}
	// Level notes
	{
		const char* p = vedMetaData[4].c_str();
		const char* p0 = p;
		const char* p1 = nullptr;
		while(*p != '\0') {
			p++;
			if(*p == '@') {
				if(p1 != nullptr) throw std::invalid_argument(
					"Level note conatains more then one tertiary seperator!");
				p1 = p;
			}
			if((*p == '$') || (*p == '\0')) {
				if(p1 == nullptr) throw std::invalid_argument(
					"Level note didn't conatain a tertiary seperator!");
				if(p1 == p0) throw std::invalid_argument(
					"Level note can't have an empty name!");
				LevelNote L(std::string(p0, p1 - p0), std::string(p1 + 1, p - p1 - 1));
				if(levelNotes.count(L.name) != 0) throw std::invalid_argument(
					"Found multiple level notes with the same name!");
				levelNotes.insert(L.pair());
				p0 = p + 1;
				p1 = nullptr;
			}
		}
	}

	// Read all scripts
	pugi::xml_node scriptData = data.child("script");
	{
		const auto addScript = [&](V6Script Sc) {
			//std::cout << "> > > [" << Sc.name << "] < < <\n";
			if(scripts.count(Sc.name) != 0) throw std::invalid_argument(
				"Found multiple scripts with the same name!");
			scripts.insert(Sc.pair());
		};
		const char* p = scriptData.child_value();
		const char* p0 = p;
		const char* p1 = p;
		std::string scriptName = "";
		if((*p == '|') || (*p == ':')) throw std::invalid_argument(
			"Script data can not start with a '|' or ':'!");
		while(*p != '\0') {
			p++;
			if(*p == '|') {
				p0 = p + 1;
			} else if((*p == ':') && (*(p + 1) == '|')) {
				if(scriptName.size() > 0) {
					addScript(V6Script(scriptName, p1, p0 - 1));
				}
				scriptName = std::string(p0, p - p0);
				p1 = p + 2;
			}
		}
		if(scriptName.size() > 0) {
			addScript(V6Script(scriptName, p1, p - 1));
		}
	}
}

void V6Level::saveAS(std::string fileName)
{
	std::ofstream Out(fileName);

	Out << "<?xml version=\"1.0\" ?>"
		   "<MapData version = \"2\">"
		   "<!--Save file-->"
		   "<Data><MetaData>\n";

	for(int i = 0; i < V6_METADATA_COUNT; i++) {
		Out << "<" << v6_MetaDataNodeNames[i] << ">" << metaData.data[i]
			<< "</" << v6_MetaDataNodeNames[i] << ">\n";
	}

	Out << "</MetaData>"
		   "<mapwidth>"
		<< sizeX << "</mapwidth>"
					"<mapheight>"
		<< sizeY << "</mapheight>"
					"<levmusic>"
		<< metaData.levelMusic << "</levmusic>\n<contents>";

	for(unsigned int yr = 0; yr < sizeY; yr++) {
		for(unsigned int yb = 0; yb < V6_ROOM_Y; yb++) {
			for(unsigned int xr = 0; xr < sizeX; xr++) {
				for(unsigned int xb = 0; xb < V6_ROOM_X; xb++) {
					Out << room(xr, yr).block(xb, yb) << ",";
				}
			}
		}
	}

	Out << "</contents>\n<edEntities>\n";

	// Entities

	for(unsigned int yr = 0; yr < sizeY; yr++) {
		for(unsigned int xr = 0; xr < sizeX; xr++) {
			room(xr, yr).printEntities(xr, yr, Out);
		}
	}

	// Ved Meta Data Entity

	Out << "<edentity x=\"800\" y=\"600\" t=\"17\" p1=\"0\" p2=\"0\" p3=\"0\" p4=\"0\" p5=\"320\" p6=\"240\">";

	for(unsigned int i = 0; i < vedMetaData.size(); i++) {
		if(i > 0) Out << '|';
		switch(i) {
			case 0:
				Out << vedVersionNumber;
				break;
			case 1:
				for(int j = 0; j < 100; j++) {
					if(j > 0) Out << "$";
					Out << flagNames[j];
				}
				break;
			case 4: {
				int first = true;
				for(const std::pair<const std::string, LevelNote>& LNP : levelNotes) {
					const LevelNote& LN = LNP.second;
					if(first) {
						first = false;
					} else {
						Out << "|";
					}
					Out << LN.name << "@" << LN.content;
				}
			} break;
			default:
				Out << vedMetaData[i];
		}
	}

	Out << "</edentity>";

	Out << "</edEntities>\n<levelMetaData>";

	// Room names and settings

	V6Room OutOfBoundsRoom = V6Room();
	for(unsigned int yr = 0; yr < V6_ROOMS_Y; yr++) {
		for(unsigned int xr = 0; xr < V6_ROOMS_X; xr++) {
			if((xr < sizeX) && (yr < sizeY)) {
				// Regular rooms
				room(xr, yr).printMetaData(Out);
			} else {
				// Out of bounds room that V6 still stores
				OutOfBoundsRoom.printMetaData(Out);
			}
		}
	}

	Out << "</levelMetaData>\n<script>";

	// Scripts

	bool first = true;
	for(const std::pair<const std::string, V6Script>& SCP : scripts) {
		if(first) {
			first = false;
		} else {
			Out << "|";
		}
		SCP.second.print(Out);
	}

	Out << "|</script>\n</Data></MapData>";

	Out.close();
}

void V6Level::insertScript(const V6Script& script)
{
	removeScript(script.name);
	scripts.insert(script.pair());
}

void V6Level::insertScript(const std::string& name, const std::vector<std::string>& lines, bool internal)
{
	insertScript(V6Script(name, lines, internal));
}

bool V6Level::hasScript(const std::string& name)
{
	return scripts.find(name) != scripts.end();
}

bool V6Level::removeScript(const std::string& name)
{
	auto it = scripts.find(name);
	if(it == scripts.end()) return false;
	scripts.erase(it);
	return true;
}
