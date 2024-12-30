#ifndef UTF8_H_DEFINED
#define UTF8_H_DEFINED

#include <string>

namespace utf8 {
	typedef unsigned int symbol;
	size_t size(const std::string& str);
	size_t size(const utf8::symbol& sym);
	utf8::symbol symbolAt(const std::string& str, size_t pos);
	std::string add(const std::string& str, const utf8::symbol& sym);
}; // namespace utf8

#endif
