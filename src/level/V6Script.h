#ifndef V6SCRIPT_H
#define V6SCRIPT_H

#include <map>
#include <string>
#include <vector>

// Store which flags are used
// on what lines of a script
struct FlagUsage {
	int onLine;
	int flagNum;
	int pattern;
	FlagUsage(int line, int num, int ptrn);
};

// Store which scripts are used
// on what lines of a script
struct ScriptUsage {
	int onLine;
	std::string scriptName;
	int pattern;
	ScriptUsage(int line, std::string name, int ptrn);
};

// A script in a V6 Level
class V6Script {
  private:
	// The content of the script line by line
	std::vector<std::string> lines{};

	// Store which flags are used on which
	// lines of the script
	std::vector<FlagUsage> flagUse{};

	// Store which scripts are used on which
	// lines of the script
	std::vector<ScriptUsage> scriptUse{};

	// Changes the number of the flag used
	// on a spcific line to the provided value
	void changeFlag(const FlagUsage& N);

	// Changes the name of a script used
	// on a spcific line to the provided value
	void changeScript(const ScriptUsage& S);

	void checkReferences();

	bool internal{false};

  public:
	const std::string name;

	V6Script() = default;
	V6Script(std::string name_, const char* pStart, const char* pStop);
	V6Script(std::string name_, const std::vector<std::string> content, bool _internal);
	V6Script(const V6Script& Sc) = default;
	V6Script(V6Script&& Sc) = default;
	~V6Script() = default;

	V6Script& operator=(const V6Script& A) = default;
	V6Script& operator=(V6Script&& A) = default;

	// Check if the script uses a certain flag
	bool usesFlag(int num);

	// Check if the script uses a certain other script
	bool usesScript(const std::string& name);

	// Swap the uses of to flags
	void flagRefactor(int n1, int n2);

	// Rename every appearence of a script name
	void scriptRefactor(const std::string& oldName, const std::string& newName);

	// Create a pair to insert this script into a map
	inline std::pair<std::string, V6Script> pair()
	{
		return std::pair<std::string, V6Script>(name, *this);
	};

	inline const std::pair<std::string, V6Script> pair() const
	{
		return std::pair<std::string, V6Script>(name, *this);
	};

	void print(std::ostream& Out) const;

	// Access to the lines
	inline const std::vector<std::string>& getLines() { return lines; };

	// Extract all textbox lines
	std::vector<std::string> extractText() const;

	// Compare scripts based on their content
	// They can however have different names
	bool operator==(const V6Script& A) const;

	static std::vector<int> internalSaySplit(const std::vector<std::string> content);

	static void appendNLines(std::vector<int>& script, int n);
};

#endif
