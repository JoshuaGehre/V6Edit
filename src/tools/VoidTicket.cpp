#include "VoidTicket.h"

#include <stdlib.h>

using namespace VoidTicket;

void VoidTicket::fullLevel(V6Level* V6)
{
	// Overworld space background
	// Read template
	uint16_t space[62][40]{};
	for(unsigned int bx = 0; bx < 62; bx++) {
		for(unsigned int by = 0; by < 40; by++) {
			space[bx][by] = V6->room(
								  13 + (bx >= 31),
								  0 + (by >= 20))
								.block(
									(bx >= 31) ? bx - 31 : bx + 9,
									(by >= 20) ? by - 20 : by + 10);
		}
	}
	// Insert space background
	for(unsigned int rx = 0; rx < 12; rx++) {
		for(unsigned int ry = 0; ry < 20; ry++) {
			V6->room(rx, ry).directmode() = 1;
			V6->room(rx, ry).tileset() = 4;
			V6->room(rx, ry).tilecol() = 5;
			for(unsigned int bx = 0; bx < 40; bx++) {
				for(unsigned int by = 0; by < 30; by++) {
					uint16_t& b = V6->room(rx, ry).block(bx, by);
					uint16_t btx = b % 40;
					uint16_t bty = b / 40;
					if((b == 0) || ((btx <= 8) && (bty >= 27))) {
						b = space[bx + 2 * rx][(by + 2 * ry) % 40];
					}
				}
			}
		}
	}
	// Ending cutscene space background
	// Read template
	uint16_t space2[44][20];
	for(unsigned int bx = 0; bx < 44; bx++) {
		for(unsigned int by = 0; by < 20; by++) {
			space2[bx][by] = V6->room(13 + (bx >= 14), 2)
								 .block((bx >= 14) ? bx - 14 : bx + 26, by + 5);
		}
	}
	// Insert background
	for(unsigned int rx = 12; rx < 20; rx++) {
		for(unsigned int ry = 17; ry < 20; ry++) {
			unsigned int offset = 14;
			if(ry == 19) {
				offset = 19 - rx;
			} else if((ry == 18) && (rx >= 13)) {
				offset = 27 - rx;
			}
			for(unsigned int bx = 0; bx < 30; bx++) {
				for(unsigned int by = 0; by < 20; by++) {
					uint16_t& b = V6->room(rx, ry).block(bx + 5, by + 5);
					uint16_t btx = b % 40;
					uint16_t bty = b / 40;
					if((b == 0) || ((btx <= 8) && (bty >= 27))) {
						b = space2[bx + offset][by];
					}
				}
			}
		}
	}
	// Randomize sign background
	for(int j = 0; j < 9; j++) {
		for(int i = 12; i < 20; i++) {
			randomizeSignBG(V6, i, j, 1);
		}
	}
	randomizeSignBG(V6, 12, 10, 8);
	randomizeSignBG(V6, 12, 11, 8);
	randomizeSignBG(V6, 12, 12, 7);
	randomizeSignBG(V6, 19, 12, 1);
	for(int i = 12; i < 18; i++) {
		randomizeSignBG(V6, i, 13, 1);
	}
	randomizeSignBG(V6, 18, 13, 2);
}

void VoidTicket::demoLevel(V6Level* V6)
{
	uint16_t space[62][40]{};
	for(unsigned int bx = 0; bx < 62; bx++) {
		for(unsigned int by = 0; by < 40; by++) {
			space[bx][by] = V6->room(
								  8 + (bx >= 31),
								  0 + (by >= 20))
								.block(
									(bx >= 31) ? bx - 31 : bx + 9,
									(by >= 20) ? by - 20 : by + 10);
		}
	}

	for(unsigned int rx = 0; rx < 6; rx++) {
		for(unsigned int ry = 0; ry < 10; ry++) {
			V6->room(rx, ry).directmode() = 1;
			V6->room(rx, ry).tileset() = 4;
			V6->room(rx, ry).tilecol() = 5;
			for(unsigned int bx = 0; bx < 40; bx++) {
				for(unsigned int by = 0; by < 30; by++) {
					uint16_t& b = V6->room(rx, ry).block(bx, by);
					uint16_t btx = b % 40;
					uint16_t bty = b / 40;
					if((b == 0) || ((btx <= 8) && (bty >= 27))) {
						b = space[bx + 4 * rx][(by + 4 * ry) % 40];
					}
				}
			}
		}
	}
}

void VoidTicket::randomizeSignBG(V6Level* V6, int rx, int ry, int dx)
{
	std::array<uint16_t, 4> lookup{360, 447, 448, 449};
	std::array<uint16_t, V6_ROOM_XY> blocks{};
	for(int i = 0; i < V6_ROOM_XY; i++) {
		blocks[i] = lookup[rand() & 3];
	}
	for(int i = 0; i < dx; i++) {
		if(V6->room(rx + i, ry).tileset() == 0) {
			for(int j = 0; j < V6_ROOM_XY; j++) {
				uint16_t& t = V6->room(rx + i, ry).block(j, 0);
				if((t == 360) || ((t >= 447) && (t <= 449))) {
					t = blocks[j];
				}
			}
		}
	}
}
