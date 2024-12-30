#include "ContentParser.h"

#include <iostream>
#include <string>

ContentParser::ContentParser(const char* x) :
	cont(x)
{}

int ContentParser::getNextId()
{
	if(eoc()) throw std::out_of_range("Content has ended!");

	int val = 0;

	while(!(eoc() || (*cont == ','))) {
		// This direct conversion much faster
		// than any sort of extracting and
		// passing strings to std::stoi
		val = 10 * val + *cont - '0';
		cont++;
	}

	if(!eoc()) cont++;

	return val;
}
