#ifndef V6METADATA_H
#define V6METADATA_H

#include <string>

#define V6_METADATA_COUNT 9

// Names of the different xml tags containing this data
extern const char* v6_MetaDataNodeNames[V6_METADATA_COUNT];

// Metadata of a V6 level
struct V6MetaData {
	// Put all data in an array
	std::string data[V6_METADATA_COUNT];

	// Access to the different types of data by name
	inline std::string& creator() { return data[0]; };
	inline std::string& title() { return data[1]; };
	inline std::string& created() { return data[2]; };
	inline std::string& modified() { return data[3]; };
	inline std::string& modifiers() { return data[4]; };
	inline std::string& desc1() { return data[5]; };
	inline std::string& desc2() { return data[6]; };
	inline std::string& desc3() { return data[7]; };
	inline std::string& website() { return data[8]; };

	// V6 does not consider this as metadata
	// but it fits better in here then in the level directly
	unsigned int levelMusic;
};

#endif
