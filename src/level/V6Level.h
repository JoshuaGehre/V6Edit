#ifndef V6LEVEL_H
#define V6LEVEL_H

#include <map>
#include <string>
#include <vector>

#include "LevelNote.h"
#include "V6MetaData.h"
#include "V6Room.h"
#include "V6Script.h"

#include "../generator/ScriptBuilder.h"

// A custom level for V6
// Can be loaded from a file
// manipulated and saved
class V6Level {
  private:
	// Number of rooms in x and y direction
	unsigned int sizeX;
	unsigned int sizeY;

	// All rooms
	std::vector<V6Room> rooms{};

	// Parts of the ved meta data
	// This contains the version number
	// flagnames, level notes and unused stuff
	std::vector<std::string> vedMetaData;

	// Ved has a version number
	int vedVersionNumber;

	// V6 has 100 flags
	// VED allows for naming these flags
	std::string flagNames[100];

	// All scripts
	std::map<std::string, V6Script> scripts;

	// Level Notes
	std::map<std::string, LevelNote> levelNotes;

  public:
	// The regular level metaData
	V6MetaData metaData;

	// Load a level from a .vvvvvv file
	V6Level(std::string fileName);
	virtual ~V6Level() = default;

	// Save the level in a .vvvvvv file
	void saveAS(std::string fileName);

	// Access a room in the level
	inline V6Room& room(int x, int y) { return rooms[x + sizeX * y]; };

	inline std::map<std::string, V6Script>& getScripts() { return scripts; };

	void insertScript(const V6Script& script);
	void insertScript(const std::string& name, const std::vector<std::string>& lines, bool internal);

	inline void insertScript(const std::string& name, const ScriptBuilder& script) { insertScript(name, script, script.internal); };
	inline void insertScript(const ScriptBuilder& script) { insertScript(script.name, script, script.internal); };

	bool hasScript(const std::string& name);
	bool removeScript(const std::string& name);

	inline int getSizeX() const { return sizeX; };
	inline int getSizeY() const { return sizeY; };
};

#endif // V6LEVEL_H
