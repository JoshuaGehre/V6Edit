#include "Freelancing.h"

using namespace Freelancing;

void Freelancing::generateTemplate(V6Level* V6)
{
	std::vector<int> ids{504, 713, 714, 0};
	int pos = 0;
	const auto& placeExample = [&](int type, int right, int up, int left, int down) {
		int inRoom = pos % 70;
		int roomX = pos / 70;
		int roomY = roomX / 5;
		roomX %= 5;
		int blockX = 2 + 4 * (inRoom % 10);
		int blockY = 2 + 4 * (inRoom / 10);
		type = ids[type];
		right = ids[right];
		up = ids[up];
		left = ids[left];
		down = ids[down];
		V6Room& R = V6->room(roomX, roomY);
		R.block(blockX, blockY) = type;
		R.block(blockX + 1, blockY) = right;
		R.block(blockX, blockY - 1) = up;
		R.block(blockX - 1, blockY) = left;
		R.block(blockX, blockY + 1) = down;
		pos++;
	};
	for(int type = 0; type <= 3; type++) {
		for(int right = 0; right <= type; right++) {
			for(int up = 0; up <= type; up++) {
				for(int left = 0; left <= type; left++) {
					for(int down = 0; down <= type; down++) {
						placeExample(type, right, up, left, down);
					}
				}
			}
		}
	}
}

void Freelancing::generateLevel(V6Level* V6, V6Level* Template)
{
	// How far each type is offset in the lookup table
	// 17 = 1 + 2^4
	// 98 = 1 + 2^4 + 3^4
	std::vector<int> lookupOffset{0, 1, 17, 98};
	// Lookup what corner to place based on the tile
	// and its 4 surrounding tiles here
	std::vector<int> lookup(354, 0);
	// This might need to be raised in the future when more than 1200 tiles are used
	// 0 White
	// 1 Magenta
	// 2 Purple
	// 3 Air
	// 4 White + Edges (Does not exist)
	// 5 Magenta + Edges
	// 6 Purple + Eges
	// 7 Air + Edges
	// -1 Other
	std::vector<int> tileTypes(1200, -1);
	tileTypes[504] = 0;
	tileTypes[713] = 1;
	tileTypes[714] = 2;
	tileTypes[0] = 3;
	int pos = 0;
	const auto& readTemplate = [&](int type) {
		int inRoom = pos % 70;
		int roomX = pos / 70;
		int roomY = roomX / 5;
		roomX %= 5;
		int blockX = 2 + 4 * (inRoom % 10);
		int blockY = 2 + 4 * (inRoom / 10);
		int id = Template->room(roomX, roomY).block(blockX, blockY);
		lookup[pos] = id;
		if(tileTypes[id] == -1) {
			tileTypes[id] = type + 4;
		}
		pos++;
	};
	for(int type = 0; type <= 3; type++) {
		for(int right = 0; right <= type; right++) {
			for(int up = 0; up <= type; up++) {
				for(int left = 0; left <= type; left++) {
					for(int down = 0; down <= type; down++) {
						readTemplate(type);
					}
				}
			}
		}
	}
	std::vector<int> altTilesLookup(1200, -1);
	std::vector<int16_t> altTiles(0);
	const auto& addAlt = [&](int b1, int b2, int b3, bool vert, bool rev) {
		altTilesLookup[b1] = 4 * altTiles.size() + (vert ? 1 : 0) + (rev ? 2 : 0);
		altTiles.push_back(b1);
		altTiles.push_back(b2);
		altTiles.push_back(b3);
	};
	for(int a = 0; a < 3; a++) {
		for(int b = 0; b < 2; b++) {
			int c = 80 * a + 7 * b;
			addAlt(880 + c, 881 + c, 882 + c, false, false);
			addAlt(922 + c, 921 + c, 920 + c, false, true);
		}
	}
	for(int a = 0; a < 2; a++) {
		for(int b = 0; b < 2; b++) {
			int c = 120 * a + 7 * b;
			addAlt(884 + c, 924 + c, 964 + c, true, false);
			addAlt(963 + c, 923 + c, 883 + c, true, true);
		}
	}
	addAlt(1124, 1164, 1162, true, false);
	addAlt(1161, 1163, 1123, true, true);
	addAlt(1131, 1171, 1167, true, false);
	addAlt(1166, 1170, 1130, true, true);
	for(unsigned int rx = 0; rx < 20; rx++) {
		for(unsigned int ry = 0; ry < 20; ry++) {
			V6Room& R = V6->room(rx, ry);
			bool hasClouds = false;
			bool hasEdges = false;
			for(unsigned int bx = 0; bx < 40; bx++) {
				for(unsigned int by = 0; by < 30; by++) {
					int b = tileTypes[R.block(bx, by)];
					switch(b) {
						case 0:
						case 1:
						case 2:
							hasClouds = true;
							break;
						case 4:
						case 5:
						case 6:
						case 7:
							hasEdges = true;
							by = 50;
							bx = 50;
							break;
					}
				}
			}
			if(hasClouds && (!hasEdges)) {
				printf("Should generate %d %d\n", rx, ry);
				generateRoom(R, lookupOffset, lookup, tileTypes, altTilesLookup, altTiles);
			}
		}
	}
}

std::array<int16_t, 40> Freelancing::altTiles(int length)
{
	std::array<int16_t, 40> r{};
	if(length < 2) {
		r[0] = 0;
	} else if(length == 2) {
		if((rand() & 3) == 0) {
			r[0] = 0;
			r[1] = 0;
		} else {
			r[0] = 1;
			r[1] = 2;
		}
	} else {
		bool sFlag = false;
		for(int i = 0; i < length; i++) {
			if((i + 1) == length) {
				r[i] = 0;
			} else if((i + 2) == length) {
				r[i] = 1;
				i++;
				r[i] = 2;
			} else if((rand() & 15) < (7 - (sFlag ? 4 : 0))) {
				r[i] = 0;
				sFlag = true;
			} else {
				r[i] = 1;
				i++;
				r[i] = 2;
				sFlag = false;
			}
		}
	}
	return r;
}

void Freelancing::altTilesX(
	std::array<std::array<int16_t, 30>, 40>& blocks,
	int by,
	int bxStart, int bxEnd,
	bool reverse,
	int16_t* variants)
{
	std::array<int16_t, 40> v = altTiles(bxEnd - bxStart + 1);
	if(reverse) {
		for(int i = bxEnd; i >= bxStart; i--) {
			blocks[i][by] = variants[v[bxEnd - i]];
		}
	} else {
		for(int i = bxStart; i <= bxEnd; i++) {
			blocks[i][by] = variants[v[i - bxStart]];
		}
	}
}

void Freelancing::altTilesY(
	std::array<std::array<int16_t, 30>, 40>& blocks,
	int bx,
	int byStart, int byEnd,
	bool reverse,
	int16_t* variants)
{
	std::array<int16_t, 40> v = altTiles(byEnd - byStart + 1);
	if(reverse) {
		for(int i = byEnd; i >= byStart; i--) {
			blocks[bx][i] = variants[v[byEnd - i]];
		}
	} else {
		for(int i = byStart; i <= byEnd; i++) {
			blocks[bx][i] = variants[v[i - byStart]];
		}
	}
}

void Freelancing::generateRoom(
	V6Room& R,
	std::vector<int>& lookupOffset,
	std::vector<int>& lookup,
	std::vector<int>& tileTypes,
	std::vector<int> altTilesLookup,
	std::vector<int16_t> altTiles)
{
	std::array<std::array<int16_t, 30>, 40> blocks;
	for(unsigned int bx = 0; bx < 40; bx++) {
		for(unsigned int by = 0; by < 30; by++) {
			int t_center = R.block(bx, by);
			int center = tileTypes[t_center];
			if(center != (center & 3)) {
				blocks[bx][by] = t_center;
				continue;
			}
			int left = tileTypes[R.block(bx > 0 ? bx - 1 : 0, by)] & 3;
			int up = tileTypes[R.block(bx, by > 0 ? by - 1 : 0)] & 3;
			int right = tileTypes[R.block(bx < 39 ? bx + 1 : 39, by)] & 3;
			int down = tileTypes[R.block(bx, by < 29 ? by + 1 : 29)] & 3;
			left = std::min(center, left);
			up = std::min(center, up);
			right = std::min(center, right);
			down = std::min(center, down);
			int x = lookupOffset[center] + down + (center + 1) * (left + (center + 1) * (up + (center + 1) * right));
			blocks[bx][by] = lookup[x];
			/*if((lookup[x] >= 1200) || lookup[x] < 0) {
				printf("Test %d %d | %d %d %d %d -> %d -> %d\n", t_center, center, right, up, left, down, x, lookup[x]);
			}*/
		}
	}
	// Horizontal replace
	for(unsigned int by = 0; by < 30; by++) {
		for(unsigned int bx = 0; bx < 40; bx++) {
			int t = blocks[bx][by];
			if((altTilesLookup[t] != -1) && ((altTilesLookup[t] & 1) == 0)) {
				unsigned int bx_;
				for(bx_ = bx + 1; bx_ < 40; bx_++) {
					if(blocks[bx_][by] != t) break;
				}
				altTilesX(blocks,
						  by, bx, bx_ - 1,
						  ((altTiles[t] & 2) == 2),
						  altTiles.data() + (altTilesLookup[t] >> 2));
			}
		}
	}
	// Vertical replace
	for(unsigned int bx = 0; bx < 40; bx++) {
		for(unsigned int by = 0; by < 30; by++) {
			int t = blocks[bx][by];
			if((altTilesLookup[t] != -1) && ((altTilesLookup[t] & 1) == 1)) {
				unsigned int by_;
				for(by_ = by + 1; by_ < 30; by_++) {
					if(blocks[bx][by_] != t) break;
				}
				altTilesY(blocks,
						  bx, by, by_ - 1,
						  ((altTiles[t] & 2) == 2),
						  altTiles.data() + (altTilesLookup[t] >> 2));
			}
		}
	}
	// Copy into final room
	for(unsigned int bx = 0; bx < 40; bx++) {
		for(unsigned int by = 0; by < 30; by++) {
			R.block(bx, by) = blocks[bx][by];
		}
	}
}
