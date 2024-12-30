#ifndef BITHELPER_H_DEFINED
#define BITHELPER_H_DEFINED

namespace BitHelper {
	// In a set represented by bits in an int, iterate over every subset
	template <typename F>
	inline void subsetIterator(unsigned int value, F function)
	{
		unsigned int i = value;
		while(i) {
			function(i);
			i = (i - 1) & value;
		}
		// Empty set
		function(0);
	}

	// Like subsetIterator but exit the loop if the function returns true
	// Return true if the loop was broken out of (even if on the last element)
	template <typename F>
	inline bool subsetIteratorBreakable(unsigned int value, F function)
	{
		unsigned int i = value;
		while(i) {
			if(function(i)) return true;
			i = (i - 1) & value;
		}
		// Empty set
		return function(0);
	}

	// Extract only the lowest bit that is on
	inline unsigned int lowestBit(unsigned int value) { return value - (value & (value - 1)); }

	// Count the number of bits that is on
	// This would be a one liner in assembly: popcnt
	inline unsigned int numberOfBitsOn(unsigned int value)
	{
		value = value - (0x55555555 & (value >> 1));
		value = (0x33333333 & value) + ((0xcccccccc & value) >> 2);
		value = (value + (value >> 4)) & 0x0f0f0f0f;
		return (0x01010101 * value) >> 24;
	}

	// For a number with only 1 bit on, get the index of the bit
	inline unsigned int indexOfBit(unsigned int value) { return numberOfBitsOn(value - 1); }

	// Iterate over the index of every bit that is on
	template <typename F>
	inline void bitPosIterator(unsigned int value, F function)
	{
		while(value) {
			int i = lowestBit(value);
			function(indexOfBit(i));
			value -= i;
		}
	}

	// Extract bits at specified positions and compact them
	// This again would be a one liner in assembly: pext
	inline unsigned int extractBits(unsigned int x, unsigned int mask)
	{
		unsigned int res = 0;
		for(unsigned int bb = 1; mask != 0; bb += bb) {
			if(x & mask & -mask) {
				res |= bb;
			}
			mask &= (mask - 1);
		}
		return res;
	}
}; // namespace BitHelper

#endif
