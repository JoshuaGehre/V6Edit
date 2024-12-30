#ifndef LEVELNOTE_H
#define LEVELNOTE_H

#include <map>
#include <string>

class LevelNote {
  private:
  public:
	// Don't change the names of the level notes
	// since the are also the keys in the map
	const std::string name;

	// Content of the level note
	std::string content;

	LevelNote(std::string name_, std::string content_);
	LevelNote(const LevelNote& L) = default;
	~LevelNote() = default;

	// Create a pair to insert this level note into a map
	inline std::pair<std::string, LevelNote> pair()
	{
		return std::pair<std::string, LevelNote>(name, *this);
	};
};

#endif
