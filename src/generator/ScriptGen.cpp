#include "ScriptGen.h"

#include <stdexcept>
#include <stdio.h>

#include "ScriptBuilder.h"

void createFlagForkRec(V6Level& V, const std::string& pattern, const std::vector<int>& flags,
					   std::function<void(const std::string&, const std::vector<bool>&)> baseCallback,
					   const std::vector<int>& symbolPosition, std::vector<bool>& state, size_t position)
{
	std::string name = ScriptGen::insertPattern(pattern, position, symbolPosition, state);
	if(position < flags.size()) {
		ScriptBuilder script(false, name);
		std::vector<std::string> scriptLines;
		for(size_t j = position; j < flags.size(); j++) {
			state[j] = true;
			std::string callScript = ScriptGen::insertPattern(pattern, j + 1, symbolPosition, state);
			script.ifflag(flags[j], callScript);
			createFlagForkRec(V, pattern, flags, baseCallback, symbolPosition, state, j + 1);
			state[j] = false;
		}
		std::string callScript = ScriptGen::insertPattern(pattern, flags.size(), symbolPosition, state);
		script.iftrinkets(callScript);
		baseCallback(callScript, state);
		V.insertScript(script);
	} else {
		baseCallback(name, state);
	}
}

void ScriptGen::createFlagFork(V6Level& V, const std::string& name, const std::vector<int>& flags,
							   std::function<void(const std::string&, const std::vector<bool>&)> baseCallback)
{
	std::vector<int> symbolPosition;
	for(size_t i = 0; i < name.size(); i++) {
		if(name.at(i) == '|') {
			symbolPosition.push_back(i);
		}
	}
	if((symbolPosition.size() != flags.size()) || (flags.size() == 0)) {
		throw std::invalid_argument("Number of flags does not match the amount of markers in the name!");
	}
	std::vector<bool> state(flags.size(), false);
	createFlagForkRec(V, name, flags, baseCallback, symbolPosition, state, 0);
}

// The name should be passed as a copy, since we want to modify it
std::string ScriptGen::insertPattern(std::string name, size_t set, const std::vector<int>& symbolPosition, const std::vector<bool>& state)
{
	for(size_t i = 0; i < symbolPosition.size(); i++) {
		name[symbolPosition[i]] = (i < set) ? ('0' + state[i]) : 'x';
	}
	return name;
}
