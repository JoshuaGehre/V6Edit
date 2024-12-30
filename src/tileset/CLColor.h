#ifndef CLCOLOR_H
#define CLCOLOR_H

#include <string>

#if defined(__linux__) // Linux implementation

	// Not yet tested
	#define CLCOLOR_SUPPORTED true

#elif defined(_WIN32) // Windows implementation

	// Have you considered switching to linux?
	#define CLCOLOR_SUPPORTED true

#else // Unknown operating system

	// If you use one of these and know
	// how to do colors in the command line
	// please let me know

	// To be safe just dont do any custom
	// command line colors
	#define CLCOLOR_SUPPORTED false

#endif

const std::string& clColor(int x);

// Combine the color from the foreground and background color
inline const std::string& clColor(int fg, int bg)
{
	return clColor(fg + bg * 0x10);
};

// Default text color
// Non bright white on black background
inline const std::string clDefaultColor()
{
	return clColor(7);
};

#endif
