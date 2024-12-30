#ifndef JJJUMP_H_DEFINED
#define JJJUMP_H_DEFINED

#include "../level/V6Level.h"
#include "../other/UTF8.h"

namespace JJJUMP {

	struct Tile {
		utf8::symbol symbol = 32;
		// Collision Types
		// 0 -> Empty
		// 1 -> Solid
		// 2 -> Spike
		int collision_base = 0;
		int player_collision[9] = {};
		bool oob = false;
		bool trigger_base = false;
		bool trigger = false;
		// Flags for one way tiles, all are marked even if they cancel out later
		// 1 -> Left
		// 2 -> Right
		// 4 -> Up
		// 8 -> Down
		unsigned char oneWay = 0;
		int collisionBits = -1;

		inline int& collision(int x, int y)
		{
			if(x < -1) x = -1;
			if(x > 1) x = 1;
			if(y < -1) y = -1;
			if(y > 1) y = 1;
			return player_collision[4 + x + 3 * y];
		}

		inline int collisionOneWayChecked(unsigned char oneWayLock)
		{
			int i = collision(0, 0);
			if(i) return i;
			return (oneWay & oneWayLock) ? 1 : 0;
		}
	};

	struct Level {
		Tile tiles[8192] = {};

		inline Tile& tile(int x, int y)
		{
			return tiles[(x & 0x7f) + 128 * (y & 0x3f)];
		}

		std::array<std::string, 16> roomColors{};
		std::array<std::string, 16> roomNames{};
		std::array<bool, 16> hasTrigger{};

		std::array<std::array<std::string, 4>, 16> edgeView{};
	};

	void loadLevel(JJJUMP::Level& J, const std::string& file);

	void diagonalCollision(
		V6Level& V6, const std::string& file, std::string marker,
		const std::string& lutName, const std::string startScript, const std::vector<int>& flags,
		const std::string& m_x, const std::string& m_xy, const std::string& m_y);

	std::vector<std::string> loadCredits(const std::string& file);

	void appendCreditFrame(
		ScriptBuilder& generator,
		const std::vector<std::string>& text,
		JJJUMP::Level& J,
		int lTiles, int position, bool backport);

	void roomScriptNormal(
		V6Level& V6, JJJUMP::Level& J,
		int rx, int ry, const std::string& name);

	void roomScriptBackport(
		V6Level& V6, JJJUMP::Level& J,
		int rx, int ry, const std::string& name);

	void generateScripts(const std::string& path, bool backPort);
}; // namespace JJJUMP

#endif
