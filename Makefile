INCLUDES = -I./ext/

CPP = g++
CFLAGS = -Wall -Wextra -Wno-implicit-fallthrough --std=c++17 -g
BUILDFLAGS = ${CFLAGS} -O3
DEBUGFLAGS =${CFLAGS} -O0 -lasan -fsanitize=address -fno-omit-frame-pointer -fsanitize=undefined

OBJECTS = $(shell find src ext -type f -name "*.cpp" | sed 's/^src/build\/release/g; s/^ext/build\/release/g; s/.cpp$$/.o/g')
DEBUGOBJECTS = $(shell find src ext -type f -name "*.cpp" | sed 's/^src/build\/debug/g; s/^ext/build\/release/g; s/.cpp$$/.o/g')

#LIBS = -lGL -lGLU -lX11 -lXxf86vm -lXrandr -lpthread -lXi -ldl -lXinerama -lXcursor -lglfw -ljsoncpp
LIBS =

DEPENDENCY_FILES = $(shell find build -type f -name "*.d" 2>/dev/null)

.PHONY: all clean format

all: $(OBJECTS)
	$(CPP) -o V6Edit $(BUILDFLAGS) $(OBJECTS) $(LIBS)

debug: $(DEBUGOBJECTS)
	$(CPP) -o V6Edit $(DEBUGFLAGS) $(DEBUGOBJECTS) $(LIBS)

-include ${DEPENDENCY_FILES}

build/release/%.o: src/%.cpp
	@mkdir -p $(dir $@)
	$(CPP) $(BUILDFLAGS) -c $< $(LIBS) -MMD -MF $@.d -o $@ $(INCLUDES)

build/release/%.o: ext/%.cpp
	@mkdir -p $(dir $@)
	$(CPP) $(BUILDFLAGS) -c $< $(LIBS) -MMD -MF $@.d -o $@ $(INCLUDES)

build/debug/%.o: src/%.cpp
	@mkdir -p $(dir $@)
	$(CPP) $(DEBUGFLAGS) -c $< $(LIBS) -MMD -MF $@.d -o $@ $(INCLUDES)

build/debug/%.o: ext/%.cpp
	@mkdir -p $(dir $@)
	$(CPP) $(DEBUGFLAGS) -c $< $(LIBS) -MMD -MF $@.d -o $@ $(INCLUDES)

clean:
	rm -rf build
	rm -f V6Edit

format:
	clang-format-14 -style=file -i $(shell find src/ -type f -regextype posix-egrep -regex ".*\.(h|cpp)$$" -print0 | xargs -0 grep -L 'Exclude from formatting')
