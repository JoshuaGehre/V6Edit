#include "JJJUMP.h"

#include <fstream>

#include "../generator/LUTGenerator.h"
#include "../generator/ScriptGen.h"

#define TICKS_PER_CREDITS_FRAME 6

void JJJUMP::loadLevel(JJJUMP::Level& J, const std::string& fileName)
{
	// Parse the textfile containing the decoration and collision of the level
	std::ifstream file(fileName);
	if(!file.is_open()) {
		printf("Could not open %s!\n", fileName.c_str());
		return;
	}
	std::string line;
	int section = 0;
	int lineNumber = 0;
	while(getline(file, line)) {
		if(line == "Hitbox:") {
			section = 1;
			lineNumber = 0;
		} else if(line == "Draw:") {
			section = 2;
			lineNumber = 0;
		} else if(line == "Rooms:") {
			section = -1;
			lineNumber = 0;
		} else if(line == "EdgeView:") {
			section = -2;
			lineNumber = 0;
		} else if((utf8::size(line) == 133) && (line.at(0) == '|') && (lineNumber < 64) && (section > 0)) {
			//printf("'%s'\n", line.c_str());
			for(int rx = 0; rx < 4; rx++) {
				for(int x = 0; x < 32; x++) {
					utf8::symbol c = utf8::symbolAt(line, 1 + rx * 33 + x);
					JJJUMP::Tile& tile = J.tile(rx * 32 + x, lineNumber);
					if(section == 1) {
						switch((unsigned char) c) {
							case '#':
								tile.collision_base = 1;
								break;
							case '^':
							case 'v':
							case '<':
							case '>':
								tile.collision_base = 2;
								break;
							case 'T':
								tile.trigger_base = true;
								break;
							case 'L':
								tile.oneWay = 0x11;
								break;
							case 'R':
								tile.oneWay = 0x22;
								break;
							case 'U':
								tile.oneWay = 0x44;
								break;
							case 'D':
								tile.oneWay = 0x88;
								break;
						}
					} else {
						tile.symbol = c;
					}
				}
			}
			lineNumber++;
		} else if((section == -1) && (line.size() >= 5) && (line.at(2) == ';')) {
			int n = line.at(0) - '0' + 4 * (line.at(1) - '0');
			std::string remainder = line.substr(3);
			size_t pos = remainder.find(";");
			if((pos != std::string::npos) && (n >= 0) && (n < 16)) {
				J.roomColors[n] = remainder.substr(0, pos);
				J.roomNames[n] = remainder.substr(pos + 1);
			}
		} else if((section == -2) && (line.size() >= 7) && (line.at(3) == '=') && (line.at(4) == '"') && (line.at(line.size() - 1) == '"')) {
			int n = line.at(0) - '0' + 4 * (line.at(1) - '0');
			int offset = (line.at(2) == 'd') ? 2 : 0;
			std::string remainder = line.substr(5, line.size() - 6);
			if((n >= 0) && (n < 16)) {
				//printf("edge %d '%s'  %lu\n", n, remainder.c_str(), utf8::size(remainder));
				std::string& str1 = J.edgeView[n][offset];
				std::string& str2 = J.edgeView[n][offset + 1];
				if(remainder == "none") {
					str1 = "                                  ";
					str2 = std::string(str1);
				} else if(utf8::size(remainder) == 34) {
					str1 = remainder;
					str2 = remainder;
				} else if(utf8::size(remainder) == 69) {
					str1 = "";
					str2 = "";
					for(int pos = 0; pos < 34; pos++) {
						str1 = utf8::add(str1, utf8::symbolAt(remainder, pos));
						str2 = utf8::add(str2, utf8::symbolAt(remainder, pos + 35));
					}
				}
			}
		}
	}
	file.close();
	// Default Colors and Roomnames
	for(int i = 0; i < 16; i++) {
		if(J.roomColors[i].size() == 0) J.roomColors[i] = "gray";
		int roomNameLength = utf8::size(J.roomNames[i]);
		int padding = (34 - roomNameLength) / 2;
		for(int j = 0; j < padding; j++) {
			J.roomNames[i] = " " + J.roomNames[i];
		}
		int paddingRight = 34 - padding - roomNameLength;
		for(int j = 0; j < paddingRight; j++) {
			J.roomNames[i] = J.roomNames[i] + " ";
		}
	}
	// Calculate the collision of each 3 by 3 area around each tile
	for(int y = 0; y < 64; y++) {
		for(int x = 0; x < 128; x++) {
			int collision = 0;
			bool trigger = false;
			unsigned char oneWay = 0;
			for(int dy = -1; dy < 2; dy++) {
				for(int dx = -1; dx < 2; dx++) {
					const Tile& Tx = J.tile(x + dx, y + dy);
					int base_collision = Tx.collision_base;
					if(((x & 0x60) == ((x + dx) & 0x60)) && ((y & 0x30) == ((y + dy) & 0x30))) {
						trigger |= Tx.trigger_base;
					}
					switch(base_collision) {
						case 1:
							collision = 1;
							break;
						case 2:
							if(collision == 0) collision = 2;
							break;
					}
					oneWay |= Tx.oneWay;
				}
			}
			if(trigger) {
				J.hasTrigger[((x >> 5) & 3) + ((y >> 2) & 0xc)] = true;
			}
			Tile& T = J.tile(x, y);
			T.collision(0, 0) = collision;
			T.trigger = trigger;
			T.oneWay = ((oneWay >> 4) & 0xf) | (T.oneWay & 0xf0);
			//if(T.oneWay) printf("OW %d %d -> %d\n", x, y, T.oneWay & 0xf);
			if(collision != 0) {
				// Mark OOB areas, where the player can't be or can't survive
				T.oob = true;
			}
		}
	}
	// For each tile calculate the neighbouring collisions
	for(int y = 0; y < 64; y++) {
		for(int x = 0; x < 128; x++) {
			Tile& T = J.tile(x, y);
			//int right = T.collision(1, 0) = J.tile(x + 1, y).collision(0, 0);
			//int left = T.collision(-1, 0) = J.tile(x - 1, y).collision(0, 0);
			//int up = T.collision(0, -1) = J.tile(x, y - 1).collision(0, 0);
			//int down = T.collision(0, 1) = J.tile(x, y + 1).collision(0, 0);
			int right = T.collision(1, 0) = J.tile(x + 1, y).collisionOneWayChecked(0x1);
			int left = T.collision(-1, 0) = J.tile(x - 1, y).collisionOneWayChecked(0x2);
			int up = T.collision(0, -1) = J.tile(x, y - 1).collisionOneWayChecked(0x8);
			int down = T.collision(0, 1) = J.tile(x, y + 1).collisionOneWayChecked(0x4);
			// Corners
			const auto& calculateCorner = [&](int dx, int dy, int lr, int ud, unsigned char oneWayLock) {
				if((lr == 1) && (ud == 1)) {
					T.collision(dx, dy) = 1;
				} else if(lr + ud == 3) {
					T.collision(dx, dy) = 2;
				} else {
					T.collision(dx, dy) = J.tile(x + dx, y + dy).collisionOneWayChecked(oneWayLock);
				}
			};
			calculateCorner(-1, -1, left, up, 0xA);
			calculateCorner(1, -1, right, up, 0x9);
			calculateCorner(-1, 1, left, down, 0x6);
			calculateCorner(1, 1, right, down, 0x5);
		}
	}
	// Collision bits for each tile
	for(int y = 0; y < 64; y++) {
		for(int x = 0; x < 128; x++) {
			Tile& T = J.tile(x, y);
			if(!T.oob) {
				T.collisionBits = T.collision(-1, -1) +
					4 * T.collision(0, -1) +
					16 * T.collision(1, -1) +
					64 * T.collision(-1, 0) +
					256 * T.collision(1, 0) +
					1024 * T.collision(-1, 1) +
					4096 * T.collision(0, 1) +
					16384 * T.collision(1, 1) +
					65536 * T.trigger;
			}
		}
	}
	// Default Edge view
	for(int ry = 0; ry < 4; ry++) {
		for(int rx = 0; rx < 4; rx++) {
			int n = rx + 4 * ry;
			bool top1 = J.edgeView[n][0].size() == 0;
			bool top2 = J.edgeView[n][1].size() == 0;
			bool bottom1 = J.edgeView[n][2].size() == 0;
			bool bottom2 = J.edgeView[n][3].size() == 0;
			if(top1 || top2 || bottom1 || bottom2) {
				for(int pos = 0; pos < 34; pos++) {
					if(top1) J.edgeView[n][0] = utf8::add(J.edgeView[n][0], J.tile(rx * 32 - 1 + pos, ry * 16 - 2).symbol);
					if(top2) J.edgeView[n][1] = utf8::add(J.edgeView[n][1], J.tile(rx * 32 - 1 + pos, ry * 16 - 1).symbol);
					if(bottom1) J.edgeView[n][2] = utf8::add(J.edgeView[n][2], J.tile(rx * 32 - 1 + pos, ry * 16 + 16).symbol);
					if(bottom2) J.edgeView[n][3] = utf8::add(J.edgeView[n][3], J.tile(rx * 32 - 1 + pos, ry * 16 + 17).symbol);
				}
			}
		}
	}
}

void JJJUMP::roomScriptNormal(
	V6Level& V6, JJJUMP::Level& J,
	int rx, int ry, const std::string& name)
{
	std::vector<std::string> scriptLines;
	scriptLines.push_back("text(" + J.roomColors[rx + 4 * ry] + ",16,24,22L)");
	std::array<std::string, 4>& edgeView = J.edgeView[rx + 4 * ry];
	scriptLines.push_back(edgeView[0]);
	scriptLines.push_back(edgeView[1]);
	for(int y = 0; y < 16; y++) {
		std::string line = "";
		for(int x = -1; x < 33; x++) {
			line = utf8::add(line, J.tile(rx * 32 + x, ry * 16 + y).symbol);
		}
		scriptLines.push_back(line);
	}
	scriptLines.push_back(edgeView[2]);
	scriptLines.push_back(edgeView[3]);
	scriptLines.push_back("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$");
	scriptLines.push_back(J.roomNames[rx + 4 * ry]);
	scriptLines.push_back("backgroundtext");
	scriptLines.push_back("speak");
	scriptLines.push_back("customiftrinkets(0,player_xxxxx_xxxx)");
	V6.insertScript(name, scriptLines, true);
}

void JJJUMP::roomScriptBackport(
	V6Level& V6, JJJUMP::Level& J,
	int rx, int ry, const std::string& name)
{
	std::vector<std::string> scriptLines;
	std::vector<std::string> scriptLinesAlt;
	std::array<std::string, 4>& edgeView = J.edgeView[rx + 4 * ry];
	const auto& pushBoth = [&](const std::string& line) {
		scriptLines.push_back(line);
		scriptLinesAlt.push_back(line);
	};
	scriptLines.push_back("customifflag(22," + name + "_alt)");
	scriptLines.push_back("text(" + J.roomColors[rx + 4 * ry] + ",16,32,11)");
	scriptLinesAlt.push_back("text(" + J.roomColors[rx + 4 * ry] + ",16,32,6)");
	pushBoth(edgeView[1]);
	for(int y = 0; y < 16; y++) {
		std::string line = "";
		for(int x = -1; x < 33; x++) {
			line = utf8::add(line, J.tile(rx * 32 + x, ry * 16 + y).symbol);
		}
		if(y == 10) {
			scriptLines.push_back("backgroundtext");
			scriptLines.push_back("speak");
			scriptLines.push_back("text(" + J.roomColors[rx + 4 * ry] + ",16,128,6)");
		} else {
			scriptLines.push_back(line);
		}
		if(y == 5) {
			scriptLinesAlt.push_back("backgroundtext");
			scriptLinesAlt.push_back("speak");
			scriptLinesAlt.push_back("text(" + J.roomColors[rx + 4 * ry] + ",16,88,11)");
		} else {
			scriptLinesAlt.push_back(line);
		}
	}
	pushBoth(edgeView[2]);
	pushBoth("backgroundtext");
	pushBoth("speak");
	pushBoth("text(" + J.roomColors[rx + 4 * ry] + ",16,192,1)");
	pushBoth(J.roomNames[rx + 4 * ry]);
	pushBoth("backgroundtext");
	pushBoth("speak");
	pushBoth("customiftrinkets(0,player_xxxxx_xxxx)");
	V6.insertScript(name, scriptLines, true);
	V6.insertScript(name + "_alt", scriptLinesAlt, true);
}

void JJJUMP::generateScripts(const std::string& path, bool backport)
{
	V6Level V6(path + "JJJUMP_Template.vvvvvv");
	JJJUMP::Level J;
	JJJUMP::loadLevel(J, path + "JJJUMP/Map.txt");

	/*for(int y = 0; y < 64; y++) {
		std::string row = "";
		for(int x = 0; x < 128; x++) {
			row = utf8::add(row, J.tile(x, y).symbol);
		}
		printf("|%s|\n", row.c_str());
	}*/

	std::vector<std::string> scriptLines;
	std::vector<int> roomFlags{10, 11, 20, 21};

	ScriptGen::createFlagFork(V6, "room_||_||", roomFlags,
							  [&](const std::string& name, const std::vector<bool>& flags) {
								  int rx = 2 * flags[0] + flags[1];
								  int ry = 2 * flags[2] + flags[3];
								  if(backport) {
									  roomScriptBackport(V6, J, rx, ry, name);
								  } else {
									  roomScriptNormal(V6, J, rx, ry, name);
								  }
							  });
	// Scripts for all collision states
	scriptLines.clear();
	for(int i = 0; i < 17; i++) {
		scriptLines.push_back("flag");
	}
	scriptLines.push_back("iftrinkets(0,end_of_frame)");

	for(int y = 0; y < 64; y++) {
		for(int x = 0; x < 128; x++) {
			int collisionBits = J.tile(x, y).collisionBits;
			std::string name = "c_" + std::to_string(collisionBits);
			if((collisionBits != -1) && !V6.hasScript(name)) {
				for(int i = 0; i < 17; i++) {
					scriptLines[i] = "flag(" + std::to_string(i + 50) + (((collisionBits & 1) == 1) ? ",on)" : ",off)");
					collisionBits >>= 1;
				}
				V6.insertScript(name, scriptLines, false);
			}
		}
	}

	// Selection scripts to pick the collision based on the tile of the room
	// this is called after the player is drawn so we know the location in the
	// room but we don't know which room the player is in
	std::string collisionLUT[32][16] = {};
	// For the LUTGenerater a little endian format is preferrable
	//roomFlags = std::vector<int>{10, 11, 20, 21};
	roomFlags = std::vector<int>{11, 10, 21, 20};
	LUTGenerator lutGen(roomFlags, "clut");
	for(int by = 0; by < 16; by++) {     // 16
		for(int bx = 0; bx < 32; bx++) { // 32
			for(int ry = 0; ry < 4; ry++) {
				for(int rx = 0; rx < 4; rx++) {
					Tile& T = J.tile(rx * 32 + bx, ry * 16 + by);
					if(!T.oob) {
						lutGen.addLookUp(rx + 4 * ry, "c_" + std::to_string(T.collisionBits));
						//printf("LUT %d %d (%d) -> %d\n", rx, ry, rx*4 + ry, T.collisionBits);
					}
				}
			}
			if(!lutGen.isUB()) {
				collisionLUT[bx][by] = lutGen.createScripts(V6);
			} else {
				collisionLUT[bx][by] = "end_of_frame";
			}
			lutGen.softReset();
			//printf("%d %d -> '%s'\n", bx, by, collisionLUT[bx][by].c_str());
		}
	}

	// Prepare script for drawing the player
	scriptLines.clear();
	scriptLines.push_back("customifflag(1,...)");
	scriptLines.push_back("text(...)");
	scriptLines.push_back("%");
	scriptLines.push_back("backgroundtext");
	scriptLines.push_back("speak");
	scriptLines.push_back("customiftrinkets(0,...)");
	//scriptLines.push_back("customifflag(...)");
	//scriptLines.push_back("customiftrinkets(...)");
	std::vector<std::string> scriptLines2;
	scriptLines2.push_back("text(...)");
	scriptLines2.push_back("%");
	scriptLines2.push_back("backgroundtext");
	scriptLines2.push_back("speak");
	scriptLines2.push_back("customiftrinkets(0,death)");

	std::vector<int> playerPositionFlags{12, 13, 14, 15, 16, 22, 23, 24, 25};
	ScriptGen::createFlagFork(V6, "player_|||||_||||", playerPositionFlags,
							  [&](const std::string& name, const std::vector<bool>& flags) {
								  int x = 16 * flags[0] + 8 * flags[1] + 4 * flags[2] + 2 * flags[3] + flags[4];
								  int y = 8 * flags[5] + 4 * flags[6] + 2 * flags[7] + flags[8];
								  std::string posStr = std::to_string(x) + "_" + std::to_string(y);
								  scriptLines[0] = "customifflag(1,death_" + posStr + ")";
								  std::string textArgs = std::to_string(8 * (3 + x)) + "," + std::to_string(8 * (5 + y)) + ",1)";
								  scriptLines[1] = "text(orange," + textArgs;
								  scriptLines[5] = "customiftrinkets(0," + collisionLUT[x][y] + ")";
								  V6.insertScript(name, scriptLines, true);
								  scriptLines2[0] = "text(red," + textArgs;
								  V6.insertScript("death_" + posStr, scriptLines2, true);
							  });
	//Diagonal Collision
	const std::string collisionFile = path + "JJJUMP/Diagonals.txt";
	diagonalCollision(V6, collisionFile, "Up + Sideways", "ul", "move_up_left",
					  std::vector<int>{56, 57, 50, 51, 52, 53}, "canceljump_dec_x", "dec_y", "sidecancel_dec_y");
	diagonalCollision(V6, collisionFile, "Up + Sideways", "ur", "move_up_right",
					  std::vector<int>{58, 59, 54, 55, 52, 53}, "canceljump_inc_x", "dec_y", "sidecancel_dec_y");
	diagonalCollision(V6, collisionFile, "Down + Sideways", "dl", "move_down_left",
					  std::vector<int>{56, 57, 60, 61, 62, 63}, "canceljump_dec_x", "inc_y", "sidecancel_inc_y");
	diagonalCollision(V6, collisionFile, "Down + Sideways", "dr", "move_down_right",
					  std::vector<int>{58, 59, 64, 65, 62, 63}, "canceljump_inc_x", "inc_y", "sidecancel_inc_y");
	// Triggers
	LUTGenerator triggerLut(roomFlags, "tlut");
	for(int i = 0; i < 16; i++) {
		if(J.hasTrigger[i]) {
			std::string triggerScript = "trigger_" + std::to_string(i & 3) + "_" + std::to_string((i >> 2) & 3);
			if(V6.hasScript(triggerScript)) {
				triggerLut.addLookUp(i, triggerScript);
			} else {
				printf("Script '%s' does not exist, but there is a corresponding trigger in that room!\n", triggerScript.c_str());
			}
		}
	}
	if(!triggerLut.isUB()) {
		std::string startLut = triggerLut.createScripts(V6);
		V6Script& sourceScript = V6.getScripts()[startLut];
		V6.removeScript("select_trigger");
		V6.insertScript("select_trigger", sourceScript.getLines(), false);
		V6.removeScript(startLut);
	}
	// Credits
	ScriptBuilder Credits(true, "credits");
	std::vector<std::string> creditsText = loadCredits(path + "JJJUMP/Credits.txt");

	int outroLength = 0;
	for(int i = 16; i >= 3; i--) {
		appendCreditFrame(Credits, creditsText, J, i, 0, backport);
		outroLength++;
	}

	for(int i = 1; i < (int) creditsText.size() - 22; i++) {
		appendCreditFrame(Credits, creditsText, J, 3, i, backport);
		outroLength++;
	}

	for(int i = 1; i < 6; i++) {
		appendCreditFrame(Credits, creditsText, J, 3, (int) creditsText.size() - 21, backport);
		outroLength++;
	}

	printf("Credits Length: %f s\n", (outroLength * TICKS_PER_CREDITS_FRAME) / 30.0);

	Credits.iftrinkets("outro");

	V6.removeScript("credits");
	V6.insertScript(Credits);

	/*V6Script& Cr = V6.getScripts()["credits"];
	for(const std::string& l : Cr.getLines()){
		printf("'%s'\n", l.c_str());
	}*/

	if(!backport) {
		V6.metaData.title() = "JJJUMP";
		V6.saveAS(path + "JJJUMP.vvvvvv");
	} else {
		V6.metaData.title() = "JJJUMP - 2.3.6 Backport";
		V6.saveAS(path + "JJJUMP_Backport.vvvvvv");
	}
}

void JJJUMP::diagonalCollision(
	V6Level& V6, const std::string& fileName, std::string marker,
	const std::string& lutName, const std::string startScript, const std::vector<int>& flags,
	const std::string& m_x, const std::string& m_xy, const std::string& m_y)
{
	LUTGenerator lutGen(flags, lutName + "lut");
	marker = "vvvvvv # " + marker;
	std::ifstream file(fileName);
	if(!file.is_open()) {
		printf("Could not open %s!\n", fileName.c_str());
		return;
	}
	std::string line;
	bool active = false;
	while(getline(file, line)) {
		if(line == marker) {
			active = true;
			//printf("Active\n");
		} else if((line.size() > 0) && (line.at(0) != ' ')) {
			active = false;
		} else if((line.size() > 10) && (line.at(7) == '-') && active) {
			int i = (line.at(5) - '0') + 4 * (line.at(3) - '0') + 16 * (line.at(1) - '0');
			line = line.substr(9);
			std::string script = "";
			if(line == "move") {
				script = m_xy;
			} else if(line == "move + die") {
				script = "die_" + m_xy;
			} else if((line == "up") || (line == "down")) {
				script = m_y;
			} else if((line == "up + die") || (line == "down + die")) {
				script = "die_" + m_y;
			} else if(line == "side") {
				script = m_x;
			} else if(line == "side + die") {
				script = "die_" + m_x;
			} else if(line == "stay") {
				script = "stay";
			}
			lutGen.addLookUp(i, script);
			//printf("%d -> %s\n", i, script.c_str());
		}
	}
	file.close();
	std::string startLut = lutGen.createScripts(V6);
	//printf("%s -> %s\n", startScript.c_str(), startLut.c_str());
	// Copy contents from generated startLut script to the intended startScript
	V6Script& sourceScript = V6.getScripts()[startLut];
	V6.removeScript(startScript);
	V6.insertScript(startScript, sourceScript.getLines(), false);
	V6.removeScript(startLut);
}

std::vector<std::string> JJJUMP::loadCredits(const std::string& fileName)
{
	std::vector<std::string> R{};
	std::ifstream file(fileName);
	if(!file.is_open()) {
		printf("Could not open %s!\n", fileName.c_str());
		return R;
	}
	std::string line;
	int align = 0;
	while(getline(file, line)) {
		if(line == "> align=left") {
			align = 0;
		} else if(line == "> align=center") {
			align = 1;
		} else if(line.rfind("> skip=") == 0) {
			int skip = atoi(line.substr(7).c_str());
			for(int i = 0; i < skip; i++) {
				R.push_back("");
			}
		} else {
			if(align == 1) {
				int pad = (26 - line.size()) / 2;
				for(int i = 0; i < pad; i++) {
					line = " " + line;
				}
			}
			R.push_back(line);
		}
	}
	return R;
}

void JJJUMP::appendCreditFrame(
	ScriptBuilder& generator,
	const std::vector<std::string>& text,
	JJJUMP::Level& J,
	int lTiles, int position, bool backport)
{
	const int xpos = 63;
	const int ypos = 14 + position;
	std::vector<std::string> lines{};
	int start = backport ? 4 : 0;
	int stop = backport ? 15 : 22;
	for(int l = start; l < stop; l++) {
		std::string line{};
		for(int i = 0; i < lTiles; i++) {
			line = utf8::add(line, J.tile(xpos + i, ypos + l).symbol);
		}
		line += "×";
		int midTiles = 32 - 2 * lTiles;
		for(int i = 0; i < midTiles; i++) {
			if((int) utf8::size(text[position + l]) > i) {
				line = utf8::add(line, utf8::symbolAt(text[position + l], i));
			} else {
				line += ' ';
			}
		}
		line += "÷";
		for(int i = 0; i < lTiles; i++) {
			line = utf8::add(line, J.tile(xpos + 34 + i - lTiles, ypos + l).symbol);
		}
		lines.push_back(line);
	}
	generator.text("yellow", 16, backport ? 56 : 24, lines);
	generator.backgroundtext();
	generator.speak(false);
	generator.delay(TICKS_PER_CREDITS_FRAME);
	generator.endtext(true);
}
