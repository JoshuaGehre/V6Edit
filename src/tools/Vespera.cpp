#include "Vespera.h"

#include <stdlib.h>

void randomizeRoom(V6Room& R)
{
	for(int bx = 0; bx < 40; bx++) {
		for(int by = 0; by < 30; by++) {
			uint16_t& b = R.block(bx, by);
			if((b == 101) || (b == 141) || (b == 222)) {
				int r = rand() % 15;
				if(r >= 2) {
					b = 101;
				} else if(r == 0) {
					b = 141;
				} else if(r == 1) {
					b = 222;
				}
			}
			if((b == 1121) || (b == 1131) || (b == 1013) || (b == 1014)) {
				int r = rand() % 4;
				if(r == 0) {
					b = 1121;
				} else if(r == 1) {
					b = 1131;
				} else if(r == 2) {
					b = 1013;
				} else if(r == 3) {
					b = 1014;
				}
			}
		}
	}
}

void mirrorRoomDecoration(V6Room& R, V6Room& Base)
{
	for(int bx = 0; bx < 40; bx++) {
		for(int by = 0; by < 30; by++) {
			uint16_t& b = R.block(bx, by);
			if((b == 101) || (b == 141) || (b == 222)) {
				b = Base.block(bx, by);
			}
			if((b == 1121) || (b == 1131) || (b == 1013) || (b == 1014)) {
				b = Base.block(bx, by);
			}
		}
	}
}

void Vespera::randomWalls(V6Level* V6)
{
	for(int rx = 0; rx < 18; rx++) {
		for(int ry = 0; ry < 13; ry++) {
			V6Room& R = V6->room(rx, ry);
			if((rx == 3) && (ry == 0)) {
				mirrorRoomDecoration(R, V6->room(2, 0));
			} else if((rx == 3) && (ry == 1)) {
				mirrorRoomDecoration(R, V6->room(2, 1));
			} else if((rx == 2) && (ry == 3)) {
				mirrorRoomDecoration(R, V6->room(2, 0));
			} else if((rx == 2) && (ry == 4)) {
				mirrorRoomDecoration(R, V6->room(2, 1));
			} else if((rx == 17) && (ry == 11)) {
				mirrorRoomDecoration(R, V6->room(16, 11));
			} else if((rx == 17) && (ry == 12)) {
				mirrorRoomDecoration(R, V6->room(16, 12));
			} else {
				randomizeRoom(R);
			}
		}
	}
}

void Vespera::copyCreditScripts(V6Level* V6, V6Level* Credits)
{
	std::map<std::string, V6Script>& levelScripts = V6->getScripts();
	std::map<std::string, V6Script>& creditScripts = Credits->getScripts();

	for(std::pair<const std::string, V6Script>& SCP : creditScripts) {
		bool doCopy = true;
		for(std::string txt : SCP.second.getLines()) {
			if(txt.find("# DO NOT COPY") != std::string::npos) {
				doCopy = false;
				break;
			}
		}
		if(doCopy) {
			if(levelScripts.find(SCP.first) != levelScripts.end()) {
				printf("Won't copy %s as it already exists!\n", SCP.first.c_str());
			} else {
				printf("Copying %s ...\n", SCP.first.c_str());
				levelScripts.insert(creditScripts[SCP.first].pair());
			}
		} else {
			printf("Should not copy %s\n", SCP.first.c_str());
		}
	}
}
