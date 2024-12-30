#include "LUTGenerator.h"

#include <stdexcept>

#include "../generator/ScriptGen.h"
#include "../other/BitHelper.h"

LUTGen::LUTGen(const std::string& _name, bool _direct, int _id) :
	isDirectCall(_direct),
	name(_name),
	id(_id)
{}

bool LUTBranch::matches(const LUTBranch& other) const
{
	if(other.distinguishedBitMask != distinguishedBitMask) return false;
	for(size_t i = 0; i < evaluatesTo.size(); i++) {
		if((other.evaluatesTo[i] != -1) && (other.evaluatesTo[i] != evaluatesTo[i])) return false;
	}
	return true;
}

void LUTBranch::merge(int position, const LUTBranch& A, const LUTBranch& B)
{
	//printf("Merge %lu and %lu\n", &A, &B);
	unsigned int bit = 1 << position;
	if((A.distinguishedBitMask != B.distinguishedBitMask) || ((A.distinguishedBitMask & bit) != 0)) {
		throw std::invalid_argument("Can only merge two LUTBranches with the same bitmask using a bit which is off!");
	}
	distinguishedBitMask = A.distinguishedBitMask + bit;
	distinguishedBitCount = BitHelper::numberOfBitsOn(distinguishedBitMask);
	evaluatesTo = std::vector<int>(1 << distinguishedBitCount, -1);
	BitHelper::subsetIterator(A.distinguishedBitMask, [&](unsigned int element) {
		int index = BitHelper::extractBits(element, A.distinguishedBitMask);
		evaluatesTo[BitHelper::extractBits(element, distinguishedBitMask)] = A.evaluatesTo[index];
		evaluatesTo[BitHelper::extractBits(element + bit, distinguishedBitMask)] = B.evaluatesTo[index];
	});
}

void LUTBranch::fill(unsigned int bitMask, int value)
{
	distinguishedBitMask = bitMask;
	distinguishedBitCount = BitHelper::numberOfBitsOn(bitMask);
	evaluatesTo = std::vector<int>(1 << distinguishedBitCount, value);
}

std::string LUTGenerator::nextFreeName()
{
	std::string r = prefix + "_" + std::to_string(claimed);
	claimed++;
	return r;
}

LUTGenerator::LUTGenerator(std::vector<int> _flags, std::string _prefix) :
	bits(_flags.size()),
	flags(_flags),
	prefix(_prefix),
	claimed(0),
	ub(true)
{
	if(bits > 30) {
		throw std::invalid_argument("LUT may not use more then 30 bits!");
	}
	state = std::vector<int>(1 << bits, -1);
}

void LUTGenerator::addLookUp(unsigned int position, const std::string& call)
{
	if(position >= state.size()) {
		throw std::invalid_argument("LUT position out of range");
	}
	const auto& it = callIdLookup.find(call);
	unsigned int id = 0;
	if(it != callIdLookup.end()) {
		id = it->second;
	} else {
		id = callScripts.size();
		callIdLookup[call] = id;
		callScripts.push_back(call);
	}
	state[position] = id;
	ub = false;
}

void LUTGenerator::addLookUp(const std::vector<bool>& position, const std::string& call)
{
	if(position.size() == bits) {
		throw std::invalid_argument("Number of LUT bits varies from position!");
	}
	unsigned int pos = 0;
	for(bool b : position) {
		pos <<= 1;
		pos += b;
	}
	addLookUp(pos, call);
}

void LUTGenerator::softReset()
{
	ub = true;
	state = std::vector<int>(1 << bits, -1);
}

int LUTGenerator::findBestSplit(unsigned int redundantBits, unsigned int bitMask)
{
	if(bitMask == 0) return -1;
	// Only initialize values for the relevant scripts
	std::vector<double> score(bits, -1);
	BitHelper::bitPosIterator(bitMask, [&](unsigned int i) {
		score[i] = 0;
	});
	// Calculate a score for each possible split
	BitHelper::subsetIterator(bitMask, [&](unsigned int element1) {
		int val1 = state[element1 + redundantBits];
		if(val1 != -1) {
			BitHelper::subsetIterator(bitMask, [&](unsigned int element2) {
				int val2 = state[element2 + redundantBits];
				unsigned int diff = element1 ^ element2;
				if((val1 != val2) && (val2 != -1)) {
					double distanceScore = 1.0 / (double) BitHelper::numberOfBitsOn(diff);
					BitHelper::bitPosIterator(diff, [&](unsigned int i) {
						score[i] += distanceScore;
						//printf("score[%d] += %f, splitting %d(%d) and %d(%d)\n", i, distanceScore, element1 + redundantBits, val1, element2 + redundantBits, val2);
					});
				}
			});
		}
	});
	/*printf("Score:");
	for(double d : score){
		printf(" %f", d);
	}
	printf("\n");*/
	// Decide what the best split is
	int maxId = -1;
	// The default max value needs to be 0 so that if all values can be eveluated
	// to the same value -1 is returned but for any positive score for any bit
	// the index of the corresponding flag is returned
	double maxValue = 0;
	BitHelper::bitPosIterator(bitMask, [&](unsigned int i) {
		if(score[i] > maxValue) {
			maxId = i;
			maxValue = score[i];
		}
	});
	//printf("Find Best Split %d %d -> %d\n", redundantBits, bitMask, maxId);
	return maxId;
}

int LUTGenerator::firstCallScript(unsigned int redundantBits, unsigned int bitMask)
{
	int call = -1;
	BitHelper::subsetIteratorBreakable(bitMask, [&](unsigned int element) -> bool {
		call = state[element + redundantBits];
		//printf("%d -> %d\n", element + redundantBits, call);
		return (call != -1);
	});
	//printf("First Call Script: %d %d -> %d (%s)\n", redundantBits, bitMask, call, callScripts[call].c_str());
	return call;
}

LUTBranch LUTGenerator::createBranch(unsigned int redundantBits, unsigned int bitMask)
{
	LUTBranch L;
	L.distinguishedBitMask = bitMask;
	L.distinguishedBitCount = BitHelper::numberOfBitsOn(bitMask);
	L.evaluatesTo = std::vector<int>(1 << L.distinguishedBitCount, -1);
	BitHelper::subsetIterator(bitMask, [&](unsigned int element) {
		L.evaluatesTo[BitHelper::extractBits(element, bitMask)] = state[element + redundantBits];
	});
	return L;
}

const LUTBranch* LUTGenerator::checkIfGenerated(const LUTBranch& L)
{
	for(const LUTBranch& i : generated) {
		if(i.matches(L)) return &i;
	}
	return nullptr;
}

LUTGen LUTGenerator::createScripts(V6Level& level, unsigned int redundantBits, unsigned bitMask)
{
	int split = findBestSplit(redundantBits, bitMask);
	if(split == -1) {
		// This entire branch is redundant we can just return the call script
		int call = firstCallScript(redundantBits, bitMask);
		return LUTGen(callScripts[call], true, call);
	} else {
		ScriptBuilder script(false, nextFreeName());
		// Generate the content of the scripts and all scripts required for it
		LUTBranch branch = createScripts(level, script, redundantBits, bitMask, split);
		branch.name = script.name;
		// Add the script to the level and to the list of generated branches
		level.insertScript(script);
		generated.push_back(branch);
		return LUTGen(script.name, false, generated.size() - 1);
	}
}

LUTBranch LUTGenerator::createScripts(V6Level& level, ScriptBuilder& script, unsigned int redundantBits, unsigned bitMask, int split)
{
	int bit = 1 << split;
	// What script should we jump to when the bit is on?
	LUTGen bitOn = createScripts(level, redundantBits + bit, bitMask - bit);
	script.ifflag(flags[split], bitOn.name);
	// Find the best split for when the bit is off
	int offCaseSplit = findBestSplit(redundantBits, bitMask - bit);
	LUTBranch offBranch;
	if(offCaseSplit == -1) {
		// The entire off branch is redundant we can just jump to the call script
		int call = firstCallScript(redundantBits, bitMask - bit);
		script.iftrinkets(callScripts[call]);
		offBranch.fill(bitMask - bit, call);
	} else {
		offBranch = createScripts(level, script, redundantBits, bitMask - bit, offCaseSplit);
	}
	//printf("On Branch %d %s %d\n", bitOn.isDirectCall, bitOn.name.c_str(), bitOn.id);
	// Merge the two LUTBranches
	LUTBranch result;
	if(bitOn.isDirectCall) {
		LUTBranch onBranch;
		onBranch.fill(bitMask - bit, bitOn.id);
		result.merge(split, offBranch, onBranch);
	} else {
		result.merge(split, offBranch, generated[bitOn.id]);
	}
	return result;
}

std::string LUTGenerator::createScripts(V6Level& level)
{
	unsigned int bitMask = (1 << bits) - 1;
	// Check if there is already a branch matching the complete branch
	const LUTBranch* S = checkIfGenerated(createBranch(0, bitMask));
	if(S) {
		return S->name;
	} else {
		return createScripts(level, 0, bitMask).name;
	}
}
