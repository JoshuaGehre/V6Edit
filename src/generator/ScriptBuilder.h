#ifndef SCRIPTBUILDER_H_DEFINED
#define SCRIPTBUILDER_H_DEFINED

#include <string>
#include <vector>

class ScriptBuilder {
  private:
	// In case of internal scripting these will show up like they would in VED
	// The surrounding squak(off) say(n) and text(1,0,0,3) is only generated
	// when the script is inserted into the level
	std::vector<std::string> lines{};

	inline void add(const std::string& line) { lines.push_back(line); };
	inline void add(const char* c_str) { lines.push_back(std::string(c_str)); };

	static int anonymousBreakpoints;

  public:
	const bool internal;
	// "custom" for internal, otherwise empty
	const std::string custom;

	// Mainly for the convenience of the user to attach a script name
	std::string name{};

	ScriptBuilder(bool _internal);
	ScriptBuilder(bool _internal, const std::string& _name);

	// Allow passing this Object instead of the line array when inserting a script
	inline operator const std::vector<std::string>&() const { return lines; };

	// Throw an exception if the script isn't internal
	void assertInternal();
	// Throw an exception if the script is internal
	void assertSimplified();

	// Append commands
	void flag(int flag, bool onOff);
	void ifflag(int flag, const std::string& call);
	void iftrinkets(int trinkets, const std::string& call);
	inline void iftrinkets(const std::string& call) { iftrinkets(0, call); }
	void iftrinketsless(int trinkets, const std::string& call);
	// 0-indexed room positions
	void warpdir(int x, int y, int warp);
	void ifwarp(int x, int y, int warp, const std::string& call);

	void text(const std::string& color, int x, int y, const std::vector<std::string>& content);
	void say(const std::vector<std::string>& content);
	void backgroundtext();
	void speak(bool active);
	void endtext(bool fast);
	void squeak(const std::string& color);

	void positionColor(const std::string& color, bool above);
	void positionCenter(bool centerX, bool centerY);

	template <typename... args>
	void text(const std::string& color, int x, int y, args... content)
	{
		std::vector<std::string> vec = {content...};
		text(color, x, y, vec);
	}

	void gotoroom(int x, int y);
	void gotoposition(int x, int y, bool flip = false);

	void map(bool onOff);
	void delay(int ticks);
	void gamestate(int i);

	void createentity(int x, int y, int e);

	void everybodysad();

	void changemood(bool sad, const char* color = "player");

	void playef(int soundId);

	// Compliant with Ally's VedFakeCommands
	void AEMTaget(int num);

	void changetile_targeted(int spriteId);
	void changecolour_targeted(int colorId);
	inline void changecolor_targeted(int colorId) { changecolour_targeted(colorId); }

	void playmusic(int num);
	void stopmusic();
	void musicfadeout();
	void musicfadein();

	void breakpoint();
};

#endif
