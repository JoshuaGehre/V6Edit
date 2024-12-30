#include "V6MetaData.h"

const char* v6_MetaDataNodeNames[V6_METADATA_COUNT] = {
	"Creator",
	"Title",
	"Created",   // No idea what these three even do
	"Modified",  // They are in every V6 level file
	"Modifiers", // Keep them just in case
	"Desc1",
	"Desc2",
	"Desc3",
	"website", // Why is this lowercase!
};
