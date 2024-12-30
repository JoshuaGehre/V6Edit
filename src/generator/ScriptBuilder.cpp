#include "ScriptBuilder.h"

#include <stdexcept>

int ScriptBuilder::anonymousBreakpoints = 0;

ScriptBuilder::ScriptBuilder(bool _internal) :
	ScriptBuilder(_internal, "")
{}

ScriptBuilder::ScriptBuilder(bool _internal, const std::string& _name) :
	internal(_internal),
	custom(_internal ? "custom" : ""),
	name(_name)
{}

void ScriptBuilder::assertInternal()
{
	if(!internal) {
		throw std::logic_error("Cannot add internal command to simplified script!");
	}
}

void ScriptBuilder::assertSimplified()
{
	if(internal) {
		throw std::logic_error("Cannot add simplified command to internal script!");
	}
}

void ScriptBuilder::flag(int flag, bool onOff)
{
	add("flag(" + std::to_string(flag) + (onOff ? ",on)" : ",off)"));
}

void ScriptBuilder::ifflag(int flag, const std::string& call)
{
	add(custom + "ifflag(" + std::to_string(flag) + "," + call + ")");
}

void ScriptBuilder::iftrinkets(int trinkets, const std::string& call)
{
	add(custom + "iftrinkets(" + std::to_string(trinkets) + "," + call + ")");
}

void ScriptBuilder::iftrinketsless(int trinkets, const std::string& call)
{
	add(custom + "iftrinketsless(" + std::to_string(trinkets) + "," + call + ")");
}

void ScriptBuilder::warpdir(int x, int y, int warp)
{
	//assertInternal();
	add("warpdir(" + std::to_string(x + 1) + "," + std::to_string(y + 1) + "," + std::to_string(warp) + ")");
}

void ScriptBuilder::ifwarp(int x, int y, int warp, const std::string& call)
{
	//assertInternal();
	add("ifwarp(" + std::to_string(x + 1) + "," + std::to_string(y + 1) + "," + std::to_string(warp) + "," + call + ")");
}

void ScriptBuilder::text(const std::string& color, int x, int y, const std::vector<std::string>& content)
{
	assertInternal();
	add("text(" + color + "," + std::to_string(x) + "," + std::to_string(y) + "," + std::to_string(content.size()) + (content.size() >= 12 ? "L)" : ")"));
	for(const std::string& s : content) {
		add(s);
	}
}

void ScriptBuilder::say(const std::vector<std::string>& content)
{
	assertSimplified();
	add("say(" + std::to_string(content.size()) + ")");
	for(const std::string& s : content) {
		add(s);
	}
}

void ScriptBuilder::backgroundtext()
{
	assertInternal();
	add("backgroundtext");
}

void ScriptBuilder::speak(bool active)
{
	assertInternal();
	add(active ? "speak_active" : "speak");
}

void ScriptBuilder::endtext(bool fast)
{
	assertInternal();
	add(fast ? "endtextfast" : "endtext");
}

void ScriptBuilder::squeak(const std::string& color)
{
	assertInternal();
	if(color == "gray") {
		add("squeak(terminal)");
	} else {
		add("squeak(" + color + ")");
	}
}

void ScriptBuilder::positionColor(const std::string& color, bool above)
{
	assertInternal();
	add("position(" + color + (above ? ",above)" : ",below)"));
}

void ScriptBuilder::positionCenter(bool centerX, bool centerY)
{
	assertInternal();
	if(centerX) {
		if(centerY) {
			add("position(center)");
		} else {
			add("position(centerx)");
		}
	} else {
		if(centerY) {
			add("position(centery)");
		} else {
			// Why are you even here?
		}
	}
}

void ScriptBuilder::gotoroom(int x, int y)
{
	assertInternal();
	add("gotoroom(" + std::to_string(x) + "," + std::to_string(y) + ")");
}

void ScriptBuilder::gotoposition(int x, int y, bool flip)
{
	assertInternal();
	add("gotoposition(" + std::to_string(x) + "," + std::to_string(y) + "," + (flip ? "1" : "0") + ")");
}

void ScriptBuilder::map(bool onOff)
{
	if(internal) {
		add(onOff ? "custommap(on)" : "custommap(off)");
	} else {
		add(onOff ? "map(on)" : "map(off)");
	}
}

void ScriptBuilder::delay(int ticks)
{
	add("delay(" + std::to_string(ticks) + ")");
}

void ScriptBuilder::gamestate(int i)
{
	assertInternal();
	add("gamestate(" + std::to_string(i) + ")");
}

void ScriptBuilder::createentity(int x, int y, int e)
{
	assertInternal();
	add("createentity(" + std::to_string(x) + "," + std::to_string(y) + "," + std::to_string(e) + ")");
}

void ScriptBuilder::everybodysad()
{
	assertInternal();
	add("everybodysad");
}

void ScriptBuilder::changemood(bool sad, const char* color)
{
	assertInternal();
	add("changecustommood(" + std::string(color) + (sad ? ",1)" : ",0)"));
}

void ScriptBuilder::playef(int soundId)
{
	assertInternal();
	add("playef(" + std::to_string(soundId) + ")");
}

void ScriptBuilder::AEMTaget(int num)
{
	assertInternal();
	add("# !MACRO! " + std::to_string(num + 4) + ", :target(" + std::to_string(num) + ")");
	add("backgroundtext");
	text("gray", 0, 0, std::vector<std::string>(num, ""));
	add("speak");
	add("endtextfast");
}

void ScriptBuilder::changetile_targeted(int spriteId)
{
	assertInternal();
	add("changetile(#," + std::to_string(spriteId) + ")");
}

void ScriptBuilder::changecolour_targeted(int colorId)
{
	assertInternal();
	add("changecolour(#," + std::to_string(colorId) + ")");
}

void ScriptBuilder::playmusic(int num)
{
	if(internal) {
		add("play(" + std::to_string(num) + ")");
	} else {
		// VVVVVV does some nonsense id conversions for song ids
		// when using the internal command, by appending an extra
		// "a" to the argument we can bypass this conversion
		add("music(" + std::to_string(num) + "a)");
	}
}

void ScriptBuilder::stopmusic()
{
	if(internal) {
		add("stopmusic");
	} else {
		add("music(0)");
	}
}

void ScriptBuilder::musicfadeout()
{
	if(internal) {
		add("musicfadeout");
	} else {
		printf("Can't generate musicfadeout in internal script '%s' stopping music instead!\n", name.c_str());
		add("music(0)");
	}
}

void ScriptBuilder::musicfadein()
{
	assertInternal();
	add("musicfadein");
}

void ScriptBuilder::breakpoint()
{
	std::vector<std::string> message{};
	if(name.size() == 0) {
		anonymousBreakpoints++;
		message.push_back("Breakpoint: " + std::to_string(anonymousBreakpoints));
	} else {
		message.push_back("Script: " + name);
	}
	message.push_back("Line: " + std::to_string(lines.size() + 1));
	if(internal) {
		text("gray", 0, 0, message);
		squeak("gray");
		positionCenter(true, true);
		speak(true);
		endtext(false);
	} else {
		say(message);
	}
}
