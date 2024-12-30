#ifndef FREELANCING_H_DEFINED
#define FREELANCING_H_DEFINED

#include "../level/V6Level.h"

namespace Freelancing {

	void generateTemplate(V6Level* V6);
	void generateLevel(V6Level* V6, V6Level* Template);
	// Functions for the level generation
	std::array<int16_t, 40> altTiles(int length);
	void altTilesX(
		std::array<std::array<int16_t, 30>, 40>& blocks,
		int by,
		int bxStart, int bxEnd,
		bool reverse,
		int16_t* variants);
	void altTilesY(
		std::array<std::array<int16_t, 30>, 40>& blocks,
		int bx,
		int byStart, int byEnd,
		bool reverse,
		int16_t* variants);
	void generateRoom(
		V6Room& R,
		std::vector<int>& lookupOffset,
		std::vector<int>& lookup,
		std::vector<int>& tileTypes,
		std::vector<int> altTilesLookup,
		std::vector<int16_t> altTiles);

}; // namespace Freelancing

#endif
