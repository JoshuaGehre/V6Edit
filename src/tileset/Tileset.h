#ifndef TILESET_H
#define TILESET_H

#include <string>

#include "../V6Properties.h"

// Tileset for displaying rooms in the command line
// Each Tile will show up as a certain ASCII character
// There are also 16 forground and 16 background colors
// which can be used in any combination
namespace TileSets {

	// The character and color for a single tile
	// Color 7 is the default white on the
	// command line with a black background
	struct Tile {
		unsigned char color = 7;
		char symbol = '?';
	};

	// A room in V6 can use one of two tilesets
	// These correlate to tiles.png and tiles2.png
	extern Tile TileSet1[1200];
	extern Tile TileSet2[1200];

	// Load a full tileset from a .v6clt file
	bool loadFromFile(Tile* arr, std::string file);

	// Save a full tileset to a .v6clt file
	void saveToFile(Tile* arr, std::string file);

	// Initialize both tileset to a default
	// This is important if no .v6clt files exist
	void initDefault();

	// Check for tiles.v6clt and tiles2.v6clt in
	// this directory load from file if they exist
	// otherwise use the default for both tilesets
	void loadCustomTiles(std::string directory);

	// Save both tilesets to tiles.v6clt and tiles2.v6clt
	// In the specfied directory
	void saveCustomTiles(std::string directory);

	// Print both tilesets into the console
	void examplePrint();
}; // namespace TileSets

std::ostream& operator<<(std::ostream& os, const TileSets::Tile& T);

#endif
