#ifndef MINESVEEPER37_H_DEFINED
#define MINESVEEPER37_H_DEFINED

#include "../level/V6Level.h"

namespace Minesveeper {

	struct Pos {
		int x;
		int y;
	};

	struct SeedDigit {
		Pos pos;
		std::vector<Pos> mine;
	};

	void generateScripts(const std::string& path);

}; // namespace Minesveeper

#endif
