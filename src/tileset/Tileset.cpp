#include "Tileset.h"

#include <fstream>
#include <iostream>
#include <vector>

#include "CLColor.h"

using namespace TileSets;

Tile TileSets::TileSet1[1200]{};
Tile TileSets::TileSet2[1200]{};

char toHexChar[17] = "0123456789abcdef";

int hexCharToInt(char c)
{
	if((c >= '0') && (c <= '9')) {
		return c - '0';
	} else if((c >= 'a') && (c <= 'f')) {
		return c - 'a' + 10;
	} else {
		return -1;
	}
}

bool loadTilesFromContent(Tile* arr, const std::vector<std::string>& Content, std::string file)
{
	const int H = 30;
	if(Content.size() != H) {
		std::cout << "Unexpected number of lines in " << file
				  << "\n\tFound: " << Content.size() << " Expected: "
				  << H << "\n";
		return false;
	}
	for(int i = 0; i < H; i++) {
		const std::string& L = Content[i];
		if(L.size() != (4 * V6_ROOM_X)) {
			std::cout << "Unexpected line length in " << file
					  << " on line " << i << "\n\tFound: " << L.size()
					  << " Expected: " << 4 * V6_ROOM_X << "\n";
			return false;
		}
		for(int j = 0; j < V6_ROOM_X; j++) {
			int bg = hexCharToInt(L.at(4 * j));
			int fg = hexCharToInt(L.at(4 * j + 1));
			char symb = L.at(4 * j + 2);
			if((bg == -1) || (fg == -1)) {
				std::cout << "Unexpected character that can't be converted to hexadecimal in "
						  << file << " on line " << i << "\n\t" << L << "\n";
				return false;
			}
			arr[j + i * V6_ROOM_X].color = (fg & 0xf) + ((bg & 0xf) << 4);
			arr[j + i * V6_ROOM_X].symbol = symb;
		}
	}
	return true;
}

bool TileSets::loadFromFile(Tile* arr, std::string file)
{
	std::ifstream In(file);
	if(!In) return false;
	std::string line;
	std::vector<std::string> Content;
	while(std::getline(In, line)) {
		if((line.size() > 0) && (line.at(0) != '#')) {
			Content.push_back(line);
		}
	}
	In.close();
	return loadTilesFromContent(arr, Content, file);
}

void TileSets::saveToFile(Tile* arr, std::string file)
{
	std::ofstream Out(file);
	for(int i = 0; i < 1200; i++) {
		int fg = arr[i].color & 0x0f;
		int bg = (arr[i].color & 0xf0) >> 4;
		char fgChar = toHexChar[fg];
		char bgChar = toHexChar[bg];

		Out << bgChar << fgChar << arr[i].symbol << ' ';

		if((i % V6_ROOM_X) == (V6_ROOM_X - 1)) Out << "\n";
	}
	Out.close();
}

void initTiles1()
{
}

void initTiles2()
{
	std::vector<std::string> Content;
#define aaa(X) Content.push_back(X)
	aaa("07  30  20  20: 02: 02. 04^ 04v 04^ 04v 70  70: 07: 07. 06u 06d 06l 06r d0  d0: 0d: 0d. f0  f0: 0f: 0f. 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? ");
	aaa("30# 30# 30# 30# 30# 30# 30# 30# 30# 04> 04< 0b> 0b< 0c> 0c< 0d> 0d< 09> 09< 0e> 0e< 0a> 0a< 0b^ 0bv 0c^ 0cv 0d^ 0dv 09^ 09v 0e^ 0ev 0a^ 0av 0c? 0c? 0c? 0c? 0c? ");

	aaa("0b# 0b# 0b# 0c# 0c# 0c# 0d# 0d# 0d# 09# 09# 09# 0e# 0e# 0e# 0a# 0a# 0a# 07# 07# 07# 02# 02# 02# 03# 03# 03# 06# 06# 06# 05# 05# 05# 01# 01# 01# 04# 04# 04# 07= ");
	aaa("0b# 0b# 0b# 0c# 0c# 0c# 0d# 0d# 0d# 09# 09# 09# 0e# 0e# 0e# 0a# 0a# 0a# 07# 07# 07# 02# 02# 02# 03# 03# 03# 06# 06# 06# 05# 05# 05# 01# 01# 01# 04# 04# 04# 07= ");
	aaa("0b# 0b# 0b# 0c# 0c# 0c# 0d# 0d# 0d# 09# 09# 09# 0e# 0e# 0e# 0a# 0a# 0a# 07# 07# 07# 02# 02# 02# 03# 03# 03# 06# 06# 06# 05# 05# 05# 01# 01# 01# 04# 04# 04# 07= ");
	aaa("0b# 0b# 0b# 0c# 0c# 0c# 0d# 0d# 0d# 09# 09# 09# 0e# 0e# 0e# 0a# 0a# 0a# 07# 07# 07# 02# 02# 02# 03# 03# 03# 06# 06# 06# 05# 05# 05# 01# 01# 01# 04# 04# 04# 07= ");
	aaa("0b# 0b# 0b# 0c# 0c# 0c# 0d# 0d# 0d# 09# 09# 09# 0e# 0e# 0e# 0a# 0a# 0a# 07# 07# 07# 02# 02# 02# 03# 03# 03# 06# 06# 06# 05# 05# 05# 01# 01# 01# 04# 04# 04# 07= ");

	aaa("0b# 0b# 0b# 0c# 0c# 0c# 0d# 0d# 0d# 09# 09# 09# 0e# 0e# 0e# 0a# 0a# 0a# f0# f0# f0# 0f# 0f# 0f# 0b# 0b# 0b# 0e# 0e# 0e# 0d# 0d# 0d# 0c# 0c# 0c# 0a# 0a# 0a# 0e# ");
	aaa("0b# 0b# 0b# 0c# 0c# 0c# 0d# 0d# 0d# 09# 09# 09# 0e# 0e# 0e# 0a# 0a# 0a# f0# f0# f0# 0f# 0f# 0f# 0b# 0b# 0b# 0e# 0e# 0e# 0d# 0d# 0d# 0c# 0c# 0c# 0a# 0a# 0a# 0c# ");
	aaa("0b# 0b# 0b# 0c# 0c# 0c# 0d# 0d# 0d# 09# 09# 09# 0e# 0e# 0e# 0a# 0a# 0a# f0# f0# f0# 0f# 0f# 0f# 0b# 0b# 0b# 0e# 0e# 0e# 0d# 0d# 0d# 0c# 0c# 0c# 0a# 0a# 0a# 0d# ");
	aaa("0b# 0b# 0b# 0c# 0c# 0c# 0d# 0d# 0d# 09# 09# 09# 0e# 0e# 0e# 0a# 0a# 0a# f0# f0# f0# 0f# 0f# 0f# 0b# 0b# 0b# 0e# 0e# 0e# 0d# 0d# 0d# 0c# 0c# 0c# 0a# 0a# 0a# 0c# ");
	aaa("0b# 0b# 0b# 0c# 0c# 0c# 0d# 0d# 0d# 09# 09# 09# 0e# 0e# 0e# 0a# 0a# 0a# f0# f0# f0# 0f# 0f# 0f# 0b# 0b# 0b# 0e# 0e# 0e# 0d# 0d# 0d# 0c# 0c# 0c# 0a# 0a# 0a# 0b# ");

	aaa("01# 01# 01# 04# 04# 04# 03# 03# 03# 05# 05# 05# 06# 06# 06# 02# 02# 02# 0d# 0d# 0d# 0c# 0c# 0c# 07= 07= 07= 07= 07= 07= 07= 07= 07= 07= 07= 07= 07= 07= 07= 07= ");
	aaa("01# 01# 01# 04# 04# 04# 03# 03# 03# 05# 05# 05# 06# 06# 06# 02# 02# 02# 0d# 0d# 0d# 0c# 0c# 0c# 07= 07= 07= 07= 07= 07= 07= 07= 07= 07= 07= 07= 07= 07= 07= 07= ");
	aaa("01# 01# 01# 04# 04# 04# 03# 03# 03# 05# 05# 05# 06# 06# 06# 02# 02# 02# 0d# 0d# 0d# 0c# 0c# 0c# 07= 07= 07= 07= 07= 07= 07= 07= 07= 07= 07= 07= 07= 07= 07= 07= ");
	aaa("01# 01# 01# 04# 04# 04# 03# 03# 03# 05# 05# 05# 06# 06# 06# 02# 02# 02# 0d# 0d# 0d# 0c# 0c# 0c# 07= 07= 07= 07= 07= 07= 07= 07= 07= 07= 07= 07= 07= 07= 07= 07= ");
	aaa("01# 01# 01# 04# 04# 04# 03# 03# 03# 05# 05# 05# 06# 06# 06# 02# 02# 02# 0d# 0d# 0d# 0c# 0c# 0c# 07= 07= 07= 07= 07= 07= 07= 07= 07= 07= 07= 07= 07= 07= 07= 07= ");
	aaa("01| 01- 01+ 04| 04- 04+ 03| 03- 03+ 05| 05- 05+ 06| 06- 06+ 02| 02- 02+ 0d| 0d- 0d+ 0c| 0c- 0c+ 07\" 07\" 07\" 07\" 07\" 07\" 07\" 07\" 07\" 07? 07? 07? 07? 07? 07? 07? ");

	aaa("03/ 03\\ 0b% 04/ 04\\ 0c% 05/ 05\\ 0d% 01/ 01\\ 09% 06/ 06\\ 0e% 02/ 02\\ 0a% 07/ 07\\ 0f% 02: 02: 02: 03: 03: 03: 06: 06: 06: 05: 05: 05: 01: 01: 01: 04: 04: 04: 07? ");
	aaa("03/ 03\\ 0b% 04/ 04\\ 0c% 05/ 05\\ 0d% 01/ 01\\ 09% 06/ 06\\ 0e% 02/ 02\\ 0a% 07/ 07\\ 0f% 02: 02: 02: 03: 03: 03: 06: 06: 06: 05: 05: 05: 01: 01: 01: 04: 04: 04: 07? ");
	aaa("03\\ 03\\ 0b% 04\\ 04\\ 0c% 05\\ 05\\ 0d% 01\\ 01\\ 09% 06\\ 06\\ 0e% 02\\ 02\\ 0a% 07\\ 07\\ 0f% 02: 02: 02: 03: 03: 03: 06: 06: 06: 05: 05: 05: 01: 01: 01: 04: 04: 04: 07? ");
	aaa("03/ 03/ 0b% 04/ 04/ 0c% 05/ 05/ 0d% 01/ 01/ 09% 06/ 06/ 0e% 02/ 02/ 0a% 07/ 07/ 0f% 02: 02: 02: 03: 03: 03: 06: 06: 06: 05: 05: 05: 01: 01: 01: 04: 04: 04: 07? ");
	aaa("07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 0a% 07? 07? 07? 02: 02: 02: 03: 03: 03: 06: 06: 06: 05: 05: 05: 01: 01: 01: 04: 04: 04: 07? ");

	aaa("07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 0a% 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? ");
	aaa("07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 0a% 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? ");
	aaa("07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 0a% 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? ");
	aaa("07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 0a% 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? ");
	aaa("07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 0a% 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? ");
	aaa("07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 0a% 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? ");
	aaa("07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 0a% 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? 07? ");
#undef aaa
	loadTilesFromContent(TileSet2, Content, "Default Tileset 2");
}

void TileSets::initDefault()
{
	initTiles1();
	initTiles2();
}

void TileSets::loadCustomTiles(std::string directory)
{
	if(!loadFromFile(TileSet1, directory + "/tiles.v6clt")) {
		initTiles1();
	}
	if(!loadFromFile(TileSet2, directory + "/tiles2.v6clt")) {
		initTiles2();
	}
}

void TileSets::saveCustomTiles(std::string directory)
{
	saveToFile(TileSet1, directory + "/tiles.v6clt");
	saveToFile(TileSet2, directory + "/tiles2.v6clt");
}

void TileSets::examplePrint()
{
	std::cout << "= = = tiles.v6clt = = =\n";
	for(int i = 0; i < 1200; i++) {
		std::cout << TileSet1[i];
		if((i % V6_ROOM_X) == (V6_ROOM_X - 1)) std::cout << "\n";
	}
	std::cout << "= = = tiles2.v6clt = = =\n";
	for(int i = 0; i < 1200; i++) {
		std::cout << TileSet2[i];
		if((i % V6_ROOM_X) == (V6_ROOM_X - 1)) std::cout << "\n";
	}
}

std::ostream& operator<<(std::ostream& os, const TileSets::Tile& T)
{
	os << clColor(T.color) << T.symbol;
	return os;
}
