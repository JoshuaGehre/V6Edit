#ifndef LUTGENERATOR_H_DEFINED
#define LUTGENERATOR_H_DEFINED

#include <map>
#include <string>
#include <vector>

#include "../level/V6Level.h"

// Result for generateScript
struct LUTGen {
	// Does this lead directly to a call script
	bool isDirectCall;
	// Script Name
	std::string name;
	// Id to either the call script or to LUTBranch
	int id;

	LUTGen(const std::string& _name, bool _direct, int _id);
};

struct LUTBranch {
	std::string name;
	// The set of bits between which we are distiguishing
	int distinguishedBitMask;
	// The number of bits that are on it the previous set
	int distinguishedBitCount;
	// The evaluation of this script
	// -1 for UB, however once generated, this should be resolved
	// and all values should be set to those that the script will
	// actually make use of
	std::vector<int> evaluatesTo;

	// This script should have all values evaluated,
	// the other script may contain UB
	bool matches(const LUTBranch& other) const;

	// Merge two LUTBranches and store it in this one
	// The bit should be the index of the relevant bit
	// In the distinguishing bit should be off in A and on in B
	void merge(int position, const LUTBranch& A, const LUTBranch& B);

	// Create a branch with a specific mask filled with a constant value
	void fill(unsigned int bitMask, int value);
};

class LUTGenerator {
  private:
	unsigned int bits;
	// FlagIds, size = bits
	std::vector<int> flags;

	// ScriptIds of called scripts, size = 2 ^ bits
	// -1 if UB is allowed for this pattern
	std::vector<int> state;
	// List of scripts that can be called
	// Script Name by Id
	std::vector<std::string> callScripts;
	// Id by Script Name
	std::map<std::string, unsigned int> callIdLookup;

	const std::string prefix;
	unsigned int claimed;

	// Generate Scripts needed for the LUT using a prefix and an attached number
	std::string nextFreeName();

	// List of previously generated scripts, with their full evaluation even
	// for elements that were allowed to be UB
	std::vector<LUTBranch> generated;

	// True if not a single callScript was set and the entire LUT is still UB
	bool ub;

	// Which of bit of the bitMask would be the best split
	// The already distiguished bits are distiguished by redundantBits
	// -1 if the call script is the same in each case, none of the bits needs to be checked
	int findBestSplit(unsigned int redundantBits, unsigned int bitMask);

	// Get the id of a callscript within a subset which is not UB
	int firstCallScript(unsigned int redundantBits, unsigned int bitMask);

	// Create the LUTBranch for a certain subset
	LUTBranch createBranch(unsigned int redundantBits, unsigned int bitMask);

	// Return the LUTBranch if a matching one was already generated
	// If no such branch exists return nullptr
	const LUTBranch* checkIfGenerated(const LUTBranch& L);

	// Creates a script for distiguishing between a given subset of bits
	LUTGen createScripts(V6Level& level, unsigned int redundantBits, unsigned int bitMask);

	// Helper function for createScripts(level, redundantBits, bitMask)
	// which adds lines to an already existing scriptbuilder
	LUTBranch createScripts(V6Level& level, ScriptBuilder& script, unsigned int redundantBits, unsigned bitMask, int split);

  public:
	LUTGenerator(std::vector<int> _flags, std::string _prefix);

	// Set the script to be called for a specific
	void addLookUp(unsigned int position, const std::string& call);
	void addLookUp(const std::vector<bool>& position, const std::string& call);

	// Reset the entire state to -1 (UB)
	// But keep all previously generated script around so that we can recycle them if needed
	void softReset();

	// Is the entire LUT UB
	inline bool isUB() const { return ub; };

	// Create the lut and return the name of the entry script
	std::string createScripts(V6Level& level);
};

#endif
