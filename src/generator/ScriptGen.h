#ifndef SCRIPTGEN_H_DEFINED
#define SCRIPTGEN_H_DEFINED

#include <functional>

#include "../level/V6Level.h"

namespace ScriptGen {
	// Creates a binary fork based on a set of flags
	// V ... Level in which to create scripts
	// name ... Pattern for the script names, using | for flag indicators
	// flags ... Id's for flags, length must be the same as | characters in the name
	// baseCallback ... Will be called with the names for the base scripts and flag states
	void createFlagFork(V6Level& V, const std::string& name, const std::vector<int>& flags,
						std::function<void(const std::string&, const std::vector<bool>&)> baseCallback);

	// Create a script name based on a pattern and the flagStates
	// This function does not check for reasonable arguments
	std::string insertPattern(std::string name, size_t set, const std::vector<int>& symbolPosition, const std::vector<bool>& state);
}; // namespace ScriptGen

#endif
