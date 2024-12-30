#include "ScriptPattern.h"

using namespace ScriptPattern;

ValidInt ScriptPattern::extractNumber(
	const std::string& str,
	const std::string& pattern)
{
	ValidString S = extractString(str, pattern);
	ValidInt R = {false, 0};
	if(S) {
		std::string strNum = S.value;
		if((strNum.size() > 0) && ((strNum[0] != '-') || (strNum.size() > 1))) {
			int val = 0;
			R.valid = true;
			for(unsigned int i = ((strNum[0] == '-') ? 1 : 0); i < strNum.size(); i++) {
				char x = strNum[i];
				if((x >= '0') && (x <= '9')) {
					val = 10 * val + x - '0';
				} else if(((x == 'l') || (x == 'L')) && (i == strNum.size() - 1)) {
					break;
				} else {
					R.valid = false;
					break;
				}
			}
			if(strNum[0] == '-') val *= -1;
			if(R.valid) R.value = val;
		}
	} else {
		R.valid = false;
	}
	return R;
}

ValidString ScriptPattern::extractString(
	const std::string& str,
	const std::string& pattern)
{
	ValidString R;
	R.valid = false;
	R.value = "";
	int sLength = str.size();
	int pLength = pattern.size();
	int s = 0;
	int p = 0;
	int lastS;
	bool earlyEnd = false;
	while(p < pLength) {
		char x = pattern[p];
		if(s == sLength) {
			if(x == '|') earlyEnd = true;
			if(earlyEnd) R.valid = true;
			return R;
		}
		switch(x) {
			case ',':
			case '(':
			case ')':
				// V6 treats these as equivalent
				// One of these characters should appear in the string
				switch(str[s]) {
					case ',':
					case '(':
					case ')':
						s++;
						p++;
						break;
					default:
						R.valid = false;
						return R;
				}
				break;
			case '%':
				// This is what we want to return
			case '*':
				// Anything can go here except these , ( or )
				lastS = s;
				while(true) {
					if(s == sLength) break;
					char y = str[s];
					if((y != ',') && (y != '(') && (y != ')') && (y != '#')) {
						s++;
					} else {
						break;
					}
				}
				if(s == lastS) {
					return R;
				} else {
					if(x == '%') {
						R.value = str.substr(lastS, s - lastS);
					}
				}
				p++;
				break;
			case '#':
				// Anything that is a number can go here
				lastS = s;
				if(str[s] == '-') {
					lastS++;
					s++;
				}
				while(true) {
					if(s == sLength) break;
					char y = str[s];
					if((y >= '0') && (y <= '9')) {
						s++;
					} else {
						break;
					}
				}
				if(s == lastS) {
					return R;
				}
				p++;
				break;
			case '|':
				earlyEnd = true;
				p++;
				break;
			case '\\':
				// Treat the next character as regular text
				// not as any of those above
				p++;
				if(p == pLength) break;
				x = pattern[p];
			default:
				// This character should appear in the text
				if(str[s] == x) {
					// Correct character found in the string
					// Move on to the next one
					p++;
					s++;
				} else {
					// Different Character found
					// Return Invalid
					return R;
				}
				break;
		}
	}
	if(s == sLength) R.valid = true;
	return R;
}

ValidString ScriptPattern::replaceSubstring(
	const std::string& str,
	const std::string& pattern,
	const std::string& newSubstr)
{
	ValidString R;
	R.valid = false;
	R.value = "";
	int sLength = str.size();
	int pLength = pattern.size();
	int s = 0;
	int p = 0;
	int lastS;
	bool earlyEnd = false;
	int substrStart = 0;
	std::string newString = "";
	auto flushSubstr = [&newString, &str, &s, &substrStart](int toPos) -> void {
		newString += str.substr(substrStart, toPos - substrStart);
		substrStart = toPos;
	};
	while(p < pLength) {
		char x = pattern[p];
		if(s == sLength) {
			if(x == '|') earlyEnd = true;
			if(earlyEnd) {
				flushSubstr(s);
				R.valid = true;
				R.value = newString;
			}
			return R;
		}
		switch(x) {
			case ',':
			case '(':
			case ')':
				// V6 treats these as equivalent
				// One of these characters should appear in the string
				switch(str[s]) {
					case ',':
					case '(':
					case ')':
						s++;
						p++;
						break;
					default:
						return R;
				}
				break;
			case '%':
				// This is what we want to replace
			case '*':
				// Anything can go here except these , ( or )
				lastS = s;
				while(true) {
					if(s == sLength) break;
					char y = str[s];
					if((y != ',') && (y != '(') && (y != ')') && (y != '#')) {
						s++;
					} else {
						break;
					}
				}
				if(s == lastS) {
					return R;
				} else {
					if(x == '%') {
						// This is where the new substring goes
						flushSubstr(lastS);
						newString += newSubstr;
						substrStart = s;
					}
				}
				p++;
				break;
			case '#':
				// Anything that is a number can go here
				lastS = s;
				if(str[s] == '-') {
					lastS++;
					s++;
				}
				while(true) {
					if(s == sLength) break;
					char y = str[s];
					if((y >= '0') && (y <= '9')) {
						s++;
					} else {
						break;
					}
				}
				if(s == lastS) {
					return R;
				}
				p++;
				break;
			case '|':
				earlyEnd = true;
				p++;
				break;
			case '\\':
				// Treat the next character as regular text
				// not as any of those above
				p++;
				if(p == pLength) break;
				x = pattern[p];
			default:
				// This character should appear in the text
				if(str[s] == x) {
					// Correct character found in the string
					// Move on to the next one
					p++;
					s++;
				} else {
					// Different Character found
					// Return Invalid
					return R;
				}
				break;
		}
	}
	if(s != sLength) return R;
	flushSubstr(s);
	R.valid = true;
	R.value = newString;
	return R;
}
