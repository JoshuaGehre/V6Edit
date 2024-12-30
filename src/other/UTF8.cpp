#include "UTF8.h"

#include <stdexcept>

using namespace utf8;

size_t utf8::size(const std::string& str)
{
	const char* c = str.c_str();
	if(c == nullptr) return 0;
	size_t length = 0;
	while(*c != 0) {
		if((((unsigned char) *c) >> 6) != 2) length++;
		c++;
	}
	return length;
}

size_t utf8::size(const utf8::symbol& sym)
{
	if(sym < 0x100) return 1;
	if(sym < 0x10000) return 2;
	if(sym < 0x1000000) return 3;
	return 4;
}

utf8::symbol utf8::symbolAt(const std::string& str, size_t pos)
{
	const char* c = str.c_str();
	size_t length = 0;
	while(*c != 0) {
		if((((unsigned char) *c) >> 6) != 2) {
			if(length == pos) {
				utf8::symbol sym = (unsigned char) *c;
				if(sym < 127) return sym;
				c++;
				while((((unsigned char) *c) >> 6) == 2) {
					sym <<= 8;
					sym += (unsigned char) *c;
					c++;
				}
				return sym;
			}
			length++;
		}
		c++;
	}
	throw std::out_of_range(("Attempted to access character at position " + std::to_string(pos) + " in a utf8 string of length " + std::to_string(length)).c_str());
}

std::string utf8::add(const std::string& str, const utf8::symbol& sym)
{
	char s[] = {0, 0, 0, 0, 0};
	switch(size(sym)) {
		case 1:
			s[0] = sym & 0xff;
			break;
		case 2:
			s[0] = (sym >> 8) & 0xff;
			s[1] = sym & 0xff;
			break;
		case 3:
			s[0] = (sym >> 16) & 0xff;
			s[1] = (sym >> 8) & 0xff;
			s[2] = sym & 0xff;
			break;
		case 4:
			s[0] = (sym >> 24) & 0xff;
			s[1] = (sym >> 16) & 0xff;
			s[2] = (sym >> 8) & 0xff;
			s[3] = sym & 0xff;
			break;
	}
	return str + s;
}
