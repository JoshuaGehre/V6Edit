#include "CLColor.h"

const static std::string clColorEmptyString = "";

// There are 256 strings in here
// Generate these at compile time using macros
const static std::string clColorStringList[256] = {
#define eachFG(BG)                                                          \
	"\033[1;30m" BG, "\033[1;31m" BG, "\033[1;32m" BG, "\033[1;33m" BG,     \
		"\033[1;34m" BG, "\033[1;35m" BG, "\033[1;36m" BG, "\033[1;37m" BG, \
		"\033[1;90m" BG, "\033[1;91m" BG, "\033[1;92m" BG, "\033[1;93m" BG, \
		"\033[1;94m" BG, "\033[1;95m" BG, "\033[1;96m" BG, "\033[1;97m" BG

	eachFG("\033[1;40m"), eachFG("\033[1;41m"), eachFG("\033[1;42m"), eachFG("\033[1;43m"),
	eachFG("\033[1;44m"), eachFG("\033[1;45m"), eachFG("\033[1;46m"), eachFG("\033[1;47m"),
	eachFG("\033[1;100m"), eachFG("\033[1;101m"), eachFG("\033[1;102m"), eachFG("\033[1;103m"),
	eachFG("\033[1;104m"), eachFG("\033[1;105m"), eachFG("\033[1;106m"), eachFG("\033[1;107m")

#undef eachFG
};

#if defined(__linux__) // Linux implementation

const std::string& clColor(int x)
{
	return clColorStringList[x & 0xff];
};

#elif defined(_WIN32) // Windows implementation

	#include <windows.h>

const static HANDLE clColorConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);

const std::string& clColor(int x)
{
	SetConsoleTextAttribute(clColorConsoleHandle, x);
	return clColorEmptyString;
};

#else // Unknown operating system

const std::string& clColor(int x)
{
	return clColorEmptyString;
};

#endif