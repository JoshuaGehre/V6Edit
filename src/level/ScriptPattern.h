#ifndef SCRIPTPATTERN_H
#define SCRIPTPATTERN_H

#include <string>

// Script Patterns:
//  , ( ) : One of these characters must also appear in the string
//  #     : Any number must appear at this point in the string
//  *     : Any non empty string can appear at this point in the
//            string, the string should be terminated by a , ( ) #
//            or the end of the string
//  |     : After this point the string may end at any point but
//            can't contradict the pattern if it doesn't
//  %     : Any string like *, however this marks the part to
//            extract or replace
//  \     : Do not interpret the next character as one of the
//            special characters above
// Any other character must appear in the string at that position

// Manipulate script lines based on patterns
namespace ScriptPattern {

	// Result when trying to extract
	// a number or string from a script line
	template <typename T>
	struct ValidVal {
		bool valid;
		T value;

		inline operator bool() const { return valid; };
	};

	typedef ValidVal<int> ValidInt;
	typedef ValidVal<std::string> ValidString;

	// Extract a number from a script line
	ValidInt extractNumber(
		const std::string& str,
		const std::string& pattern);

	// Extract a string from a script line
	ValidString extractString(
		const std::string& str,
		const std::string& pattern);

	// Replace a specific part of a string
	// This will return invalid if the initial
	// string does not match the pattern
	ValidString replaceSubstring(
		const std::string& str,
		const std::string& pattern,
		const std::string& newSubstr);
} // namespace ScriptPattern

#endif