#ifndef CONTENTPARSER_H
#define CONTENTPARSER_H

// Small class to help reading
// the blocks in a level
class ContentParser {
  private:
	const char* cont;

  public:
	ContentParser(const char* x);
	int getNextId();
	inline bool eoc() { return *cont == '\0'; };
};

#endif