#include "Minesveeper37.h"

#include <algorithm>
#include <fstream>

#include "../generator/MusicDefines.h"
#include "../generator/ScriptGen.h"
#include "../generator/SoundDefines.h"

// 0 for 16x16 minesveeper tiles
// 1 for 18x18 minesveeper tiles
// 2 for 20x20, but I find them too large
#define TILE_OFFSET 1

std::vector<std::string> loadTextFile(const std::string& fileName)
{
	std::vector<std::string> content;
	std::ifstream file(fileName);
	if(!file.is_open()) {
		printf("Could not open %s!\n", fileName.c_str());
		return content;
	}
	std::string line;
	while(getline(file, line))
		content.push_back(line);
	file.close();
	return content;
}

void Minesveeper::generateScripts(const std::string& path)
{
	V6Level* V6 = new V6Level(path + "Minesveeper_37_Template.vvvvvv");

	// First pattern
	/*std::vector<SeedDigit> seed{
		{{16, 18}, {{3, 1}, {0, 2}, {2, 5}, {4, 3}, {7, 4}}},
		{{17, 18}, {{0, 4}, {1, 1}, {3, 4}, {5, 2}, {7, 3}}},
		{{18, 18}, {{2, 0}, {4, 1}, {6, 4}, {1, 3}, {3, 5}}},
		{{19, 18}, {{0, 1}, {2, 3}, {4, 2}, {5, 5}, {6, 3}}},
		{{16, 19}, {{0, 5}, {1, 2}, {3, 0}, {4, 4}, {5, 3}}},
		{{17, 19}, {{0, 0}, {1, 4}, {3, 2}, {4, 5}, {6, 2}}},
		{{18, 19}, {{4, 0}, {2, 2}, {3, 3}, {1, 5}, {6, 5}, {7, 2}}},
		{{19, 19}, {{1, 0}, {2, 1}, {0, 3}, {2, 4}, {5, 4}, {7, 5}}},
	};*/

	std::vector<SeedDigit> seed{
		{{16, 18}, {{0, 5}, {5, 5}, {4, 4}, {7, 5}, {7, 2}}},
		{{17, 18}, {{1, 0}, {4, 0}, {1, 3}, {3, 3}, {7, 3}}},
		{{18, 18}, {{0, 0}, {4, 1}, {1, 4}, {4, 3}, {6, 4}}},
		{{19, 18}, {{1, 1}, {3, 1}, {2, 3}, {2, 5}, {5, 4}}},
		{{16, 19}, {{2, 1}, {6, 2}, {1, 5}, {4, 5}, {6, 5}}},
		{{17, 19}, {{2, 0}, {0, 2}, {5, 2}, {6, 3}, {2, 4}}},
		{{18, 19}, {{0, 1}, {0, 3}, {2, 2}, {4, 2}, {5, 3}, {3, 5}}},
		{{19, 19}, {{3, 0}, {1, 2}, {3, 2}, {0, 4}, {3, 4}, {7, 4}}},
	};

	std::vector<std::vector<int>> permData{
		// 0 1 2 3 4
		{1, 2, 3, 4, 0},
		{2, 4, 1, 0, 3},
		{4, 3, 0, 2, 1},
		// 0 1 2 3 4 5
		{1, 2, 3, 4, 5, 0},
		{2, 3, 4, 0, 5, 1},
		{5, 4, 3, 0, 2, 1}};

	std::vector<Pos> Neighbours{{1, 0}, {1, -1}, {0, -1}, {-1, -1}, {-1, 0}, {-1, 1}, {0, 1}, {1, 1}};

	// Print Seed for Debugging
	for(size_t i = 0; i < seed.size(); i++) {
		ScriptBuilder select(true, "print_debug_" + std::to_string(i));
		for(size_t j = 0; j < seed[i].mine.size(); j++) {
			std::string printName = "print_debug_" + std::to_string(i) + "_" + std::to_string(j);
			select.ifwarp(seed[i].pos.x, seed[i].pos.y, j, printName);
			ScriptBuilder seedPrint(true, printName);
			std::vector<std::string> message{std::string(1, (char) ('A' + i)) + "=" + std::to_string(j)};
			seedPrint.text("green", 16, 16 + 24 * i, message);
			seedPrint.backgroundtext();
			seedPrint.speak(false);
			seedPrint.iftrinkets("print_debug_" + std::to_string(i + 1));
			V6->insertScript(seedPrint);
		}
		std::vector<std::string> messageOOB{std::string(1, (char) ('A' + i)) + " out of bounds"};
		select.text("red", 16, 16 + 32 * i, messageOOB);
		select.backgroundtext();
		select.speak(false);
		select.iftrinkets("print_debug_" + std::to_string(i + 1));
		V6->insertScript(select);
	}

	// = = = = Permutation = = = =
	// Filter out needed permutation load scripts
	std::vector<std::string> permuteScripts{};
	std::vector<int> permutateReturn{};

	for(int x = 0; x < 5; x++) {
		for(int y = 0; y < 5; y++) {
			for(V6Entity& E : V6->room(x, y).getEntities()) {
				const std::string& name = E.str;
				if(name.size() != 21 || name.rfind("load_permute_", 0) != 0) continue;
				bool valid = true;
				for(int i = 13; i < 21; i++) {
					char c = name.at(i);
					if(c < '0' || c > '3') valid = false;
				}
				if(!valid) continue;
				permuteScripts.push_back(name);
				permutateReturn.push_back(x + 5 * y);
			}
		}
	}

	// Create Permutation Load and Setup Scripts
	for(size_t x = 0; x < permuteScripts.size(); x++) {
		const std::string& name = permuteScripts[x];
		ScriptBuilder loadPerm(false, name);
		loadPerm.iftrinkets(name.substr(5));
		V6->insertScript(loadPerm);
		ScriptBuilder setupPerm(true, name.substr(5));
		for(int i = 0; i < 8; i++) {
			setupPerm.warpdir(seed[i].pos.x, seed[i].pos.y - 4, name.at(i + 13) - '0');
		}
		setupPerm.warpdir(16, 16, permutateReturn[x]);
		setupPerm.iftrinkets("perm_sel_0");
		V6->insertScript(setupPerm);
		//printf("%s\n", name.c_str());
	}

	// Actual Permuatation Scripts
	for(int i = 0; i < 8; i++) {
		ScriptBuilder selectType(false, "perm_sel_" + std::to_string(i));
		for(int t = 1; t < 4; t++) {
			std::string permName = "perm_sel_" + std::to_string(i) + "_" + std::to_string(t);
			selectType.ifwarp(seed[i].pos.x, seed[i].pos.y - 4, t, permName);
			ScriptBuilder perm(false, permName);
			for(size_t j = 0; j < seed[i].mine.size(); j++) {
				int next = permData[t + ((seed[i].mine.size() == 5) ? -1 : 2)][j];
				perm.ifwarp(seed[i].pos.x, seed[i].pos.y, j, "perm_set_" + std::to_string(i) + "_" + std::to_string(next));
			}
			V6->insertScript(perm);
		}
		for(size_t j = 0; j < seed[i].mine.size(); j++) {
			ScriptBuilder permSet(false, "perm_set_" + std::to_string(i) + "_" + std::to_string(j));
			permSet.warpdir(seed[i].pos.x, seed[i].pos.y, j);
			permSet.iftrinkets("perm_sel_" + std::to_string(i + 1));
			V6->insertScript(permSet);
		}
		selectType.iftrinkets("perm_sel_" + std::to_string(i + 1));
		V6->insertScript(selectType);
	}

	// Permutate Return Scripts
	ScriptBuilder permSel8(false, "perm_sel_8");
	for(int x = 0; x < 5; x++) {
		for(int y = 0; y < 5; y++) {
			int t = x + 5 * y;
			if(std::find(std::begin(permutateReturn), std::end(permutateReturn), t) == std::end(permutateReturn)) continue;
			//printf("%i %i %i\n", x, y, t);
			std::string returnName = "perm_return" + std::to_string(t);
			permSel8.ifwarp(16, 16, t, returnName);
			ScriptBuilder permReturn(true, returnName);
			permReturn.gotoroom(x, y);
			permReturn.gamestate(0);
			V6->insertScript(permReturn);
		}
	}
	V6->insertScript(permSel8);

	// Generator Script
	ScriptBuilder generate(true, "generate");
	// Anti Save Load
	generate.warpdir(5, 0, 1);
	generate.playmusic(MUSIC_POTENTIAL_FOR_ANYTHING);
	generate.flag(1, true);
	for(int x = 0; x < 8; x++) {
		for(int y = 0; y < 6; y++) {
			// Neighbouring Mines
			generate.warpdir(x, y + 14, 0);
			// Display State (1 = Closed)
			generate.warpdir(x + 8, y + 14, 1);
			// Is mine
			generate.warpdir(x, y + 8, 0);
			// Flood Fill Check
			generate.warpdir(x + 8, y + 8, 0);
		}
	}
	generate.iftrinkets("put_mine_0");
	V6->insertScript(generate);

	for(int x = 0; x < 8; x++) {
		ScriptBuilder putMine(true, "put_mine_" + std::to_string(x));
		std::string next = "put_mine_" + std::to_string(x + 1);
		if(x == 7) next = "calculateneighbours";
		for(size_t i = 0; i < seed[x].mine.size(); i++) {
			int mx = seed[x].mine[i].x;
			int my = seed[x].mine[i].y;
			putMine.warpdir(mx, my + 8, 1);
			putMine.warpdir(mx + 8, my + 8, 1);
			if(i == seed[x].mine.size() - 1) break;
			putMine.ifwarp(seed[x].pos.x, seed[x].pos.y, i, next);
			putMine.warpdir(mx, my + 8, 0);
			putMine.warpdir(mx + 8, my + 8, 0);
		}
		putMine.iftrinkets(next);
		V6->insertScript(putMine);
	}

	// Calculate Neighbours
	ScriptBuilder calcInit(true, "calculateneighbours");
	calcInit.warpdir(6, 1, 0);
	calcInit.warpdir(7, 1, 0);
	calcInit.iftrinkets("cn_select_mine");
	V6->insertScript(calcInit);

	// Main Loop for selecting mines
	ScriptBuilder calcSelectMine(true, "cn_select_mine");
	for(int i = 0; i < 8; i++) {
		const SeedDigit& seedDigit = seed[i];
		std::string selectMineName = "cn_select_pos_m" + std::to_string(i);
		calcSelectMine.ifwarp(6, 1, i, selectMineName);
		// For each mine select its actual position
		ScriptBuilder selectMinePos(true, selectMineName);
		for(int j = 0; j < (int) seedDigit.mine.size(); j++) {
			std::string selectDirName = "cn_select_dir_m" + std::to_string(i) + std::to_string(j);
			if(j == (int) seedDigit.mine.size() - 1) {
				selectMinePos.iftrinkets(selectDirName);
			} else {
				selectMinePos.ifwarp(seedDigit.pos.x, seedDigit.pos.y, j, selectDirName);
			}
			// Select the direction into which we want to increment the neighbouring value
			const Pos& minePos = seedDigit.mine[j];
			ScriptBuilder selectDir(true, selectDirName);
			int dirskipped = 0;
			for(int dir = 0; dir < 8; dir++) {
				int x = minePos.x + Neighbours[dir].x;
				int y = minePos.y + Neighbours[dir].y;
				if((x < 0) || (y < 0) || (x > 7) || (y > 5)) continue;
				selectDir.ifwarp(7, 1, dirskipped, "increment_n_" + std::to_string(x) + "_" + std::to_string(y));
				dirskipped++;
			}
			selectDir.warpdir(7, 1, 0);
			selectDir.warpdir(6, 1, i + 1);
			selectDir.iftrinkets("cn_select_pos_m" + std::to_string(i + 1));
			V6->insertScript(selectDir);
		}
		V6->insertScript(selectMinePos);
	}
	calcSelectMine.iftrinkets("exit");
	V6->insertScript(calcSelectMine);

	// Increment Mine Count
	for(int x = 0; x < 8; x++) {
		for(int y = 0; y < 6; y++) {
			std::string posName = "n_" + std::to_string(x) + "_" + std::to_string(y);
			ScriptBuilder increment(false, "increment_" + posName);
			for(int num = 0; num < 8; num++) {
				std::string incName = "set_" + posName + "_" + std::to_string(num + 1);
				increment.ifwarp(x, y + 14, num, incName);
				ScriptBuilder setNeighbours(false, incName);
				if(num == 0) setNeighbours.warpdir(x + 8, y + 8, 1);
				setNeighbours.warpdir(x, y + 14, num + 1);
				setNeighbours.iftrinkets("inc_dir");
				V6->insertScript(setNeighbours);
			}
			increment.iftrinkets("inc_dir");
			V6->insertScript(increment);
		}
	}
	// Increment Direction
	ScriptBuilder incDir(true, "inc_dir");
	for(int dir = 0; dir < 8; dir++) {
		std::string nextDirName = "setdir_" + std::to_string(dir + 1);
		if(dir == 7) {
			incDir.iftrinkets(nextDirName);
		} else {
			incDir.ifwarp(7, 1, dir, nextDirName);
		}
		ScriptBuilder nextDir(true, nextDirName);
		nextDir.warpdir(7, 1, dir + 1);
		nextDir.iftrinkets("cn_select_mine");
		V6->insertScript(nextDir);
	}
	V6->insertScript(incDir);

	// Generate each tile, set the next coordinates and then switch into the corresponding AEM script
	for(int x = 0; x < 8; x++) {
		for(int y = 0; y < 6; y++) {
			std::string posName = std::to_string(x) + "_" + std::to_string(y);
			ScriptBuilder render_xy(true, "render_" + posName);
			render_xy.everybodysad();
			render_xy.createentity(24 + 32 * x - TILE_OFFSET, 44 + 32 * y - TILE_OFFSET, 21);
			int nextX = x + 1;
			int nextY = y;
			if(nextX == 8) {
				nextX = 0;
				nextY++;
				if(nextY == 6) nextY = 0;
			}
			render_xy.warpdir(6, 0, nextX);
			render_xy.warpdir(7, 0, nextY);
			render_xy.ifwarp(8 + x, 14 + y, 0, "render_open_" + posName);
			render_xy.ifwarp(x, 8 + y, 1, "render_closed_" + posName);
			render_xy.flag(3, true);
			render_xy.iftrinkets("render_closed_" + posName);
			V6->insertScript(render_xy);

			ScriptBuilder open_xy(true, "render_open_" + posName);
			open_xy.ifwarp(x, y + 8, 1, "aem_mine");
			for(int n = 0; n < 8; n++) {
				open_xy.ifwarp(x, y + 14, n, "aem_n" + std::to_string(n));
			}
			open_xy.iftrinkets("aem_n8");
			V6->insertScript(open_xy);

			ScriptBuilder closed_xy(true, "render_closed_" + posName);
			closed_xy.ifwarp(8 + x, 14 + y, 1, "aem_closed");
			closed_xy.iftrinkets("aem_flag");
			V6->insertScript(closed_xy);
		}
	}

	// Select Render X
	ScriptBuilder render_x_x(true, "render_x_x");
	for(int x = 0; x < 7; x++) {
		render_x_x.ifwarp(6, 0, x, "render_" + std::to_string(x) + "_x");
	}
	render_x_x.iftrinkets("render_7_x");
	V6->insertScript(render_x_x);

	// Select Render Y
	for(int x = 0; x < 8; x++) {
		ScriptBuilder render_n_x(true, "render_" + std::to_string(x) + "_x");
		for(int y = 0; y < 5; y++) {
			std::string next = "render_" + std::to_string(x) + "_" + std::to_string(y);
			if((x == 0) && (y == 0)) {
				// All tiles have been rendered, continue with whatever is next
				next = "render_tiles_done";
			}
			render_n_x.ifwarp(7, 0, y, next);
		}
		render_n_x.iftrinkets("render_" + std::to_string(x) + "_5");
		V6->insertScript(render_n_x);
	}
	// Create Tile AEM Scripts
	const auto& tileAEM = [&](std::string name, int spriteId, int colorId) {
		ScriptBuilder aem(true, "aem_" + name);
		aem.changetile_targeted(spriteId);
		aem.changecolor_targeted(colorId);
		if(name == "mine") aem.flag(4, true);
		if(name == "closed" || name == "flag") aem.ifflag(14, "aem_final");
		aem.iftrinkets("render_x_x");
		V6->insertScript(aem);
	};
	tileAEM("closed", 48, 21);
	tileAEM("flag", 49, 21);
	tileAEM("mine", 50, 1);
	tileAEM("n0", 36, 14);
	tileAEM("n1", 37, 16);
	tileAEM("n2", 38, 13);
	tileAEM("n3", 39, 6);
	tileAEM("n4", 40, 31);
	tileAEM("n5", 41, 39);
	tileAEM("n6", 42, 11);
	tileAEM("n7", 43, 22);
	tileAEM("n8", 44, 102);
	ScriptBuilder aemFinal(true, "aem_final");
	aemFinal.changetile_targeted(49);
	aemFinal.changecolor_targeted(7);
	aemFinal.iftrinkets("render_x_x");
	V6->insertScript(aemFinal);
	// Create simplified scripts for the tiles which select between
	// left and right click based on flag zero
	for(int x = 0; x < 8; x++) {
		for(int y = 0; y < 6; y++) {
			std::string xy = std::to_string(x) + "_" + std::to_string(y);
			ScriptBuilder tile(false, "tile_" + xy);
			tile.ifflag(7, "exit");
			tile.ifflag(0, "rightclick_" + xy);
			tile.iftrinkets("leftclick_" + xy);
			V6->insertScript(tile);

			ScriptBuilder rightclick(true, "rightclick_" + xy);
			rightclick.ifwarp(8 + x, 14 + y, 1, "flagon_" + xy);
			rightclick.ifwarp(8 + x, 14 + y, 2, "flagoff_" + xy);
			V6->insertScript(rightclick);

			ScriptBuilder flagon(true, "flagon_" + xy);
			flagon.warpdir(8 + x, 14 + y, 2);
			flagon.flag(9, true);
			flagon.playef(SOUND_SAVE);
			flagon.iftrinkets("reload");
			V6->insertScript(flagon);

			ScriptBuilder flagoff(true, "flagoff_" + xy);
			flagoff.warpdir(8 + x, 14 + y, 1);
			flagoff.playef(SOUND_SAVE);
			flagoff.iftrinkets("reload");
			V6->insertScript(flagoff);

			ScriptBuilder leftclick(true, "leftclick_" + xy);
			// Exit if we are clicking on an already open tile or a flag
			leftclick.ifwarp(8 + x, 14 + y, 0, "exit");
			leftclick.ifwarp(8 + x, 14 + y, 2, "exit");
			// Open the tile and redraw
			leftclick.warpdir(8 + x, 14 + y, 0);
			leftclick.playef(SOUND_CRUMBLE);
			leftclick.ifwarp(x + 8, y + 8, 1, "reload");
			leftclick.warpdir(6, 0, x);
			leftclick.warpdir(7, 0, y);
			leftclick.warpdir(7, 1, 0);
			leftclick.flag(8, false);
			leftclick.iftrinkets("floodfill");
			V6->insertScript(leftclick);
		}
	}
	// Render complete
	ScriptBuilder render_done(true, "render_tiles_done");
	render_done.changemood(false);
	render_done.ifflag(5, "gameover");
	render_done.ifflag(4, "open_mine_0");
	render_done.ifflag(3, "exit");
	render_done.ifflag(14, "victory");
	render_done.flag(14, true);
	render_done.gotoroom(2, 2);
	render_done.iftrinkets("digflagicon");
	V6->insertScript(render_done);

	for(size_t i = 0; i < seed.size(); i++) {
		ScriptBuilder openmine(true, "open_mine_" + std::to_string(i));
		std::string nextOpen = "open_mine_" + std::to_string(i + 1);
		if(i == seed.size() - 1) {
			nextOpen = "digflagicon";
			openmine.gotoroom(2, 2);
			openmine.flag(5, true);
		}
		const Pos& seedPos = seed[i].pos;
		for(size_t j = 0; j < seed[i].mine.size(); j++) {
			std::string openName = "open_mine_" + std::to_string(i) + "_" + std::to_string(j);
			const Pos& minePos = seed[i].mine[j];
			ScriptBuilder openmine_n(true, openName);
			openmine_n.ifwarp(minePos.x + 8, minePos.y + 14, 2, nextOpen);
			openmine_n.warpdir(minePos.x + 8, minePos.y + 14, 0);
			openmine_n.iftrinkets(nextOpen);
			V6->insertScript(openmine_n);

			if(j == seed[i].mine.size() - 1) {
				openmine.iftrinkets(openName);
			} else {
				openmine.ifwarp(seedPos.x, seedPos.y, j, openName);
			}
		}

		V6->insertScript(openmine);
	}
	// Floodfill
	ScriptBuilder floodfill(false, "floodfill");
	ScriptBuilder ff_open(false, "ffo");
	//ff_open.breakpoint();
	for(int x = 0; x < 8; x++) {
		floodfill.ifwarp(6, 0, x, "ff_" + std::to_string(x));
		ScriptBuilder ffx(false, "ff_" + std::to_string(x));
		for(int y = 0; y < 6; y++) {
			std::string xy = std::to_string(x) + "_" + std::to_string(y);
			ffx.ifwarp(7, 0, y, "ff_" + xy);
			ScriptBuilder ffxy(false, "ff_" + xy);
			if(y < 5) {
				ffxy.warpdir(7, 0, y + 1);
			} else {
				ffxy.warpdir(6, 0, x + 1);
				ffxy.warpdir(7, 0, 0);
			}
			ffxy.ifwarp(x + 8, y + 8, 1, "floodfill");  // Floodfill already executed
			ffxy.ifwarp(x + 8, y + 14, 1, "floodfill"); // Closed (Normal)
			ffxy.ifwarp(x + 8, y + 14, 2, "floodfill"); // Closed (Flag)
			//ffxy.breakpoint();
			ffxy.warpdir(x + 8, y + 8, 1);
			ffxy.warpdir(8, 0, x * 6 + y);
			ffxy.warpdir(7, 1, 0);
			ffxy.iftrinkets("ffo_" + xy);
			ff_open.ifwarp(8, 0, x * 6 + y, "ffo_" + xy);

			ScriptBuilder ffoxy(false, "ffo_" + xy);
			//ffoxy.breakpoint();
			int dirskipped = 0;
			for(int dir = 0; dir < 8; dir++) {
				int xi = x + Neighbours[dir].x;
				int yi = y + Neighbours[dir].y;
				if((xi < 0) || (yi < 0) || (xi > 7) || (yi > 5)) continue;
				ffoxy.ifwarp(7, 1, dirskipped, "ffc_" + std::to_string(xi) + "_" + std::to_string(yi));
				dirskipped++;
			}
			ffoxy.iftrinkets("floodfill");

			V6->insertScript(ffxy);
			V6->insertScript(ffoxy);

			ScriptBuilder ffcxy(false, "ffc_" + xy);
			//ffcxy.breakpoint();
			ffcxy.ifwarp(x + 8, y + 14, 0, "inc_dir_ff");
			ffcxy.warpdir(x + 8, y + 14, 0);
			//ffcxy.breakpoint();
			ffcxy.ifwarp(x + 8, y + 8, 1, "inc_dir_ff");
			ffcxy.flag(8, true);
			//ffcxy.breakpoint();
			ffcxy.iftrinkets("inc_dir_ff");
			V6->insertScript(ffcxy);
		}
		V6->insertScript(ffx);
	}
	floodfill.ifflag(8, "floodfill_repeat");
	floodfill.iftrinkets("reload");
	V6->insertScript(floodfill);

	ff_open.warpdir(6, 0, 8);
	ff_open.warpdir(7, 0, 0);
	//ff_open.breakpoint();
	ff_open.iftrinkets("floodfill");
	V6->insertScript(ff_open);

	ScriptBuilder floodfill_repeat(false, "floodfill_repeat");
	floodfill_repeat.warpdir(6, 0, 0);
	floodfill_repeat.warpdir(7, 0, 0);
	floodfill_repeat.warpdir(7, 1, 0);
	floodfill_repeat.flag(8, false);
	floodfill_repeat.iftrinkets("floodfill");
	V6->insertScript(floodfill_repeat);

	// Increment direction for floodfill
	ScriptBuilder incDirFF(false, "inc_dir_ff");
	for(int dir = 0; dir < 7; dir++) {
		std::string nextDirName = "setdirff_" + std::to_string(dir + 1);
		incDirFF.ifwarp(7, 1, dir, nextDirName);

		ScriptBuilder nextDir(false, nextDirName);
		nextDir.warpdir(7, 1, dir + 1);
		//nextDir.breakpoint();
		nextDir.iftrinkets("ffo");
		V6->insertScript(nextDir);
	}
	incDirFF.iftrinkets("floodfill");
	V6->insertScript(incDirFF);

	// Finalize the decoration
	for(int rx = 0; rx < 5; rx++) {
		for(int ry = 0; ry < 5; ry++) {
			V6Room& R = V6->room(rx, ry);
			for(unsigned int bx = 0; bx < 40; bx++) {
				for(unsigned int by = 0; by < 30; by++) {
					int b = R.block(bx, by);
					if((b < 80) || (b > 82 && b < 120) || (b > 128)) continue;
					R.block(bx, by) = b * 2 + (bx & 1) + 40 * (by & 1);
				}
			}
		}
	}
	// Copy of the main room for the tutorial
	V6Room& Game = V6->room(2, 2);
	V6Room& Tutorial = V6->room(2, 0);
	for(unsigned int bx = 0; bx < 40; bx++) {
		for(unsigned int by = 0; by < 30; by++) {
			Tutorial.block(bx, by) = Game.block(bx, by);
		}
	}
	// Clear all rooms outside of the main 5 by 5 area
	for(int rx = 0; rx < 20; rx++) {
		for(int ry = 0; ry < 20; ry++) {
			if(rx < 5 && ry < 5) continue;
			V6Room& R = V6->room(rx, ry);
			for(unsigned int bx = 0; bx < 40; bx++) {
				for(unsigned int by = 0; by < 30; by++) {
					R.block(bx, by) = 0;
				}
			}
			R.getEntities().clear();
		}
	}

	//Tutorial
	std::vector<std::string> tutorial;
	std::vector<std::string> text = loadTextFile(path + "Minesveeper_37/Tutorial.txt");
	bool digMode = true;
	for(size_t l = 0; l < text.size(); l++) {
		const std::string& line = text[l];
		if(line.size() == 0) continue;
		//printf("'%s'\n", line.c_str());
		if(line.at(0) == '>') {
			tutorial.push_back(line.substr(1));
		} else if(line == "Drawgrid") {
			tutorial.push_back("gotoroom(2,0)");
			for(int y = 0; y < 6; y++) {
				for(int x = 0; x < 8; x++) {
					char c = text[l + 1 + y][x];
					tutorial.push_back("everybodysad");
					tutorial.push_back("createentity(" + std::to_string(24 + 32 * x - TILE_OFFSET) + "," + std::to_string(44 + 32 * y - TILE_OFFSET) + ",21)");
					int tileId = 0;
					int colorId = 0;
#define SetAEM(A, B, C) \
	case A:             \
		tileId = B;     \
		colorId = C;    \
		break;
					switch(c) {
						SetAEM('0', 36, 14);
						SetAEM('1', 37, 16);
						SetAEM('2', 38, 13);
						SetAEM('3', 39, 6);
						SetAEM('4', 40, 31);
						SetAEM('5', 41, 39);
						SetAEM('6', 42, 11);
						SetAEM('7', 43, 22);
						SetAEM('8', 44, 102);
						SetAEM('C', 48, 21);
						SetAEM('F', 49, 21);
						SetAEM('M', 50, 1);
					}
					tutorial.push_back("changetile(#," + std::to_string(tileId) + ")");
					tutorial.push_back("changecolour(#," + std::to_string(colorId) + ")");
				}
			}
			tutorial.push_back("everybodysad");
			tutorial.push_back("createentity(" + std::to_string(288 - TILE_OFFSET) + "," + std::to_string(132 - TILE_OFFSET) +  ",21)");
			if(digMode) {
				tutorial.push_back("changetile(#,45)");
				tutorial.push_back("changecolour(#,39)");
			} else {
				tutorial.push_back("changetile(#,46)");
				tutorial.push_back("changecolour(#,21)");
			}
			l += 6;
		} else if(line == "PSSE") {
			tutorial.push_back("position(centerx)");
			tutorial.push_back("squeak(red)");
			tutorial.push_back("speak_active");
			tutorial.push_back("endtext");
		} else if(line == "Dig") {
			digMode = true;
		} else if(line == "Flag") {
			digMode = false;
		} else if(line == "Highlight") {
			tutorial.push_back("everybodysad");
			int x = std::stoi(text[l + 1]);
			int y = std::stoi(text[l + 2]);
			tutorial.push_back("createentity(" + std::to_string(16 + 32 * x) + "," + std::to_string(36 + 32 * y) + ",13,9,9)");
			tutorial.push_back("changetile(#,56)");
			tutorial.push_back("changecolour(#,7)");
			l += 2;
		} else if(line == "HighlightMarker") {
			tutorial.push_back("everybodysad");
			tutorial.push_back("createentity(280,124,13,9,9)");
			tutorial.push_back("changetile(#,56)");
			tutorial.push_back("changecolour(#,7)");
		}
	}
	V6->insertScript("tutorial", tutorial, true);

	// Sheep Animation
	ScriptBuilder sheepAnimation(true, "sheep_animation");
	int frameCount = 0;
	//sheepAnimation.breakpoint();
	const auto& createAnimationFrame = [&](int x, int sheepSprite) {
		std::string name = "sheep_ani_" + std::to_string(frameCount);
		sheepAnimation.ifwarp(10, 0, frameCount, name);
		frameCount++;
		ScriptBuilder frame(true, name);
		frame.warpdir(10, 0, frameCount);
		frame.gotoroom(1, 1);
		frame.gamestate(0);
		int y = 16;
		if(sheepSprite < 5) y -= (sheepSprite & 2) >> 1;
		if(x > -32) {
			frame.everybodysad();
			frame.createentity(x, y, 21);
			frame.changetile_targeted(168 + sheepSprite);
			frame.changecolor_targeted(22);
			frame.everybodysad();
			frame.createentity(x, y, 21);
			frame.changetile_targeted(156 + sheepSprite);
			frame.changecolor_targeted(19);
			frame.changemood(false);
		}
		V6->insertScript(frame);
	};

	int sheepX = 200;
	createAnimationFrame(sheepX, 7);
	createAnimationFrame(sheepX, 7);
	createAnimationFrame(sheepX, 8);
	createAnimationFrame(sheepX, 8);
	createAnimationFrame(sheepX, 9);
	createAnimationFrame(sheepX, 9);
	createAnimationFrame(sheepX, 6);
	for(int i = 0; i < 4; i++) {
		createAnimationFrame(sheepX, 5);
	}
	for(int i = 0; i < 25; i++) {
		createAnimationFrame(sheepX - i * 10, 1 + (i & 3));
	}

	sheepAnimation.flag(13, true);
	V6->insertScript(sheepAnimation);

	V6->metaData.title() = "Minesveeper 37";
	V6->saveAS(path + "Minesveeper_37.vvvvvv");
	delete V6;
}
