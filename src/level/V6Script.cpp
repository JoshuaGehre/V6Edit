#include "V6Script.h"
#include "ScriptPattern.h"

#include <iostream>

#define PATTERN_FLAG_CUSTOMIFFLAG       0
#define PATTERN_FLAG_FLAG               1
#define PATTERN_FLAG_IFFLAG             2
#define PATTERN_LINES_SAY               3
#define PATTERN_LINES_TEXT              4
#define PATTERN_SCRIPT_CUSTOMIFFLAG     5
#define PATTERN_SCRIPT_CUSTOMIFTRINKETS 6
#define PATTERN_SCRIPT_IFFLAG           7
#define PATTERN_SCRIPT_IFTRINKETS       8
#define PATTERN_SCRIPT_IFWARP           9

std::string commandPatterns[11] = {
	"customifflag(%,*|)",
	"flag(%,*|)",
	"ifflag(%,*|)",
	"say(%|,*)",
	"text(*,#,#,%|)",
	"customifflag(#,%|)",
	"customiftrinkets(#,%|)",
	"ifflag(#,%|)",
	"iftrinkets(#,%|)",
	"ifwarp(#,#,#,%|)"};

using namespace ScriptPattern;

void replaceString(std::string& subject, const std::string& search, const std::string& replace)
{
	size_t pos = 0;
	while((pos = subject.find(search, pos)) != std::string::npos) {
		subject.replace(pos, search.size(), replace);
		pos += replace.size();
	}
}

V6Script::V6Script(std::string name_, const char* pStart, const char* pStop) :
	internal(false),
	name(name_)
{
	//bool namePrinted = false;
	const char* p = pStart;
	const char* p0 = pStart;
	while(p < pStop) {
		p++;
		if((*p == '|') || (p == pStop)) {
			lines.push_back(std::string(p0, p - p0));
			p0 = p + 1;
		}
	}
	if((lines.size() > 0) && (lines[0] == "squeak(off) #v")) {
		internal = true;
	}
	/*printf("%s:\n", name_.c_str());
	for(std::string s : lines){
		printf("\t\"%s\"\n", s.c_str());
	}*/
	checkReferences();
}

V6Script::V6Script(std::string name_, const std::vector<std::string> content, bool _internal) :
	internal(_internal),
	name(name_)
{
	if(content.size() == 0) internal = false;
	if(internal) {
		std::vector<int> saySplit = internalSaySplit(content);
		//bool stopScript = false;
		if((content.back().size() < 20) || (content.back().substr(0, 19) != "customiftrinkets(0,")) {
			//stopScript = true;
			appendNLines(saySplit, 1);
		}
		/*if(saySplit.size() > 1) {
			printf("%s", name_.c_str());
			for(int i : saySplit) {
				printf(" %d", i);
			}
			printf("\n");
		}*/
		lines.push_back("squeak(off) #v");
		/*lines.push_back("say(" + std::to_string(content.size() + stopScript) + ") #v");
		for(const std::string& s : content) {
			if(s == "") {
				lines.push_back("#");
			} else {
				lines.push_back(s);
			}
		}
		// If the last line guarantees branching a loadscript(stop) is not needed
		if(stopScript) {
			lines.push_back("loadscript(stop) #v");
		}
		lines.push_back("text(1,0,0,3) #v");*/
		size_t offset = 0;
		for(unsigned int b = 0; b < saySplit.size(); b++) {
			int sayBlock = saySplit[b];
			lines.push_back("say(" + std::to_string(sayBlock + (b < saySplit.size() - 1)) + ") #v");
			for(int i = 0; i < sayBlock; i++) {
				size_t index = offset + i;
				if(index < content.size()) {
					lines.push_back(content[index]);
				} else {
					lines.push_back("loadscript(stop) #v");
				}
			}
			lines.push_back("text(1,0,0,3) #v");
			offset += sayBlock;
		}
	} else {
		for(const std::string& s : content) {
			lines.push_back(s);
		}
		if((content.size() != 0) && (content.back().size() != 0)) {
			// Append an empty line if a simplified script doesn't already end on one
			lines.push_back("");
		}
	}
	for(std::string& line : lines) {
		replaceString(line, "&", "&amp;");
		replaceString(line, "<", "&lt;");
		replaceString(line, ">", "&gt;");
		replaceString(line, "'", "&apos;");
		replaceString(line, "  ", " &#32;");
	}
	checkReferences();
}

std::vector<int> V6Script::internalSaySplit(const std::vector<std::string> content)
{
	std::vector<int> Result{};
	int skip = 0;
	for(const std::string& line : content) {
		if(skip) {
			skip--;
		} else {
			ValidInt RInt = extractNumber(line, commandPatterns[PATTERN_LINES_TEXT]);
			if(RInt) {
				skip = RInt.value;
				appendNLines(Result, skip + 1);
			} else {
				appendNLines(Result, 1);
			}
		}
	}
	return Result;
}

void V6Script::appendNLines(std::vector<int>& script, int n)
{
	if(n == 0) return;
	if(script.size()) {
		int last = script.back();
		if(n + last < 49) {
			script.back() += n;
		} else if(last > 49) {
			// Yeah, we need 2.4 either way
			// But let's still not make these too long
			if(last > 10) {
				script.push_back(n);
			} else {
				script.back() += last;
			}
		} else {
			// We can still have pre 2.4 compatability
			script.push_back(n);
		}
	} else {
		script.push_back(n);
	}
}

void V6Script::checkReferences()
{
	// Check for flag usage
	int skip = 0;
	for(unsigned int i = 0; i < lines.size(); i++) {
		ValidInt RInt;
		ValidString RStr;
		if(skip <= 0) {
			const std::string& line = lines[i];
#define checkFlagPattern(A)                         \
	RInt = extractNumber(line, commandPatterns[A]); \
	if(RInt) flagUse.push_back(FlagUsage(i, RInt.value, A));
#define checkScriptPattern(A)                       \
	RStr = extractString(line, commandPatterns[A]); \
	if(RStr) scriptUse.push_back(ScriptUsage(i, RStr.value, A));
			switch(line[0]) {
				case 'c':
					// customifflag
					checkFlagPattern(PATTERN_FLAG_CUSTOMIFFLAG);
					checkScriptPattern(PATTERN_SCRIPT_CUSTOMIFFLAG);
					// customiftrinkets
					checkScriptPattern(PATTERN_SCRIPT_IFTRINKETS);
					break;
				case 'f':
					// flag
					checkFlagPattern(PATTERN_FLAG_FLAG);
					break;
				case 'i':
					// ifflag
					checkFlagPattern(PATTERN_FLAG_IFFLAG);
					checkScriptPattern(PATTERN_SCRIPT_IFFLAG);
					// iftrinkets
					checkScriptPattern(PATTERN_SCRIPT_CUSTOMIFTRINKETS);
					// ifwarp
					checkScriptPattern(PATTERN_SCRIPT_IFWARP);
					break;
				case 's':
					// say
					RInt = extractNumber(line, commandPatterns[PATTERN_LINES_SAY]);
					if(RInt) skip = RInt.value;
					break;
				case 't':
					// text
					RInt = extractNumber(line, commandPatterns[PATTERN_LINES_TEXT]);
					if(RInt) skip = RInt.value;
					break;
			}
#undef checkFlagPattern
#undef checkScriptPattern
		} else {
			skip--;
			/*const std::string& line = lines[i];
			if(line.size() > 0){
				if(!namePrinted){
					printf("\n%s:\n\t", name_.c_str());
					namePrinted = true;
				}
				printf("%s ", line.c_str());	
				if(skip == 0){
					printf("\n\t");
				}
			}*/
		}
	}
}

void V6Script::changeFlag(const FlagUsage& N)
{
	ValidString newLine = replaceSubstring(
		lines[N.onLine],
		commandPatterns[N.pattern],
		std::to_string(N.flagNum));
	if(newLine) {
		lines[N.onLine] = newLine.value;
	} else {
		throw std::runtime_error(("Flag Refactor Failed!\n\tOld Line: " + lines[N.onLine] + "\n\tPattern: " + commandPatterns[N.pattern] +
								  "\n\tNew Value: " + std::to_string(N.flagNum) + "\n")
									 .c_str());
	}
}

void V6Script::changeScript(const ScriptUsage& S)
{
	ValidString newLine = replaceSubstring(
		lines[S.onLine],
		commandPatterns[S.pattern],
		S.scriptName);
	if(newLine) {
		lines[S.onLine] = newLine.value;
	} else {
		throw std::runtime_error(("Script Refactor Failed!\n\tOld Line: " + lines[S.onLine] + "\n\tPattern: " + commandPatterns[S.pattern] +
								  "\n\tNew Value: " + S.scriptName + "\n")
									 .c_str());
	}
}

FlagUsage::FlagUsage(int line, int num, int ptrn) :
	onLine(line),
	flagNum(num),
	pattern(ptrn)
{}

ScriptUsage::ScriptUsage(int line, std::string name, int ptrn) :
	onLine(line),
	scriptName(name),
	pattern(ptrn)
{}

bool V6Script::usesFlag(int num)
{
	for(FlagUsage& F : flagUse) {
		if(F.flagNum == num) return true;
	}
	return false;
}

bool V6Script::usesScript(const std::string& name)
{
	for(ScriptUsage& S : scriptUse) {
		if(S.scriptName == name) return true;
	}
	return false;
}

void V6Script::flagRefactor(int n1, int n2)
{
	for(FlagUsage& F : flagUse) {
		if(F.flagNum == n1) {
			F.flagNum = n2;
			changeFlag(F);
		} else if(F.flagNum == n2) {
			F.flagNum = n1;
			changeFlag(F);
		}
	}
}

void V6Script::scriptRefactor(const std::string& oldName, const std::string& newName)
{
	for(ScriptUsage& S : scriptUse) {
		if(S.scriptName == oldName) {
			S.scriptName = newName;
			changeScript(S);
		}
	}
}

void V6Script::print(std::ostream& Out) const
{
	Out << name << ':';
	for(const std::string& S : lines) {
		Out << '|' << S;
	}
}

std::vector<std::string> V6Script::extractText() const
{
	std::vector<std::string> text;
	int skip = 0;
	bool lineReady = false;
	for(unsigned int i = 0; i < lines.size(); i++) {
		ValidInt RInt;
		ValidString RStr;
		if(skip <= 0) {
			const std::string& line = lines[i];
			switch(line[0]) {
				case 's':
					// say
					RInt = extractNumber(line, commandPatterns[PATTERN_LINES_SAY]);
					if(RInt) skip = RInt.value;
					break;
				case 't':
					// text
					RInt = extractNumber(line, commandPatterns[PATTERN_LINES_TEXT]);
					if(RInt) skip = RInt.value;
					break;
			}
		} else {
			skip--;
			const std::string& line = lines[i];
			if(line.size() > 0) {
				if(lineReady) {
					text.back() += " " + line;
				} else {
					text.push_back(line);
					lineReady = true;
				}
				if(skip == 0) {
					lineReady = false;
				}
			}
		}
	}
	return text;
}

bool V6Script::operator==(const V6Script& A) const
{
	// Flag and Script use result from the lines
	// No comparison for these needed
	if(lines.size() != A.lines.size()) return false;
	for(int i = A.lines.size() - 1; i >= 0; i++) {
		if(lines[i] != A.lines[i]) return false;
	}
	return true;
}
