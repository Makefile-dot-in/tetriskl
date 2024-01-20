.PHONY: all clean
.SUFFIXES:

CXX_SOURCES = \
src/main.cpp \
src/dirs.cpp \
src/tetro.cpp \
src/game.cpp \
src/render.cpp \
src/menu.cpp

OBJECTS = $(patsubst src/%.cpp,build/%.o,$(CXX_SOURCES))
LDLIB = -lsfml-system -lsfml-window -lsfml-graphics

all: build/tetriskl

clean:
	rm -r build/*

build/tetriskl: $(OBJECTS)
	$(CXX) $(LDFLAGS) $^ -o $@ $(LDLIB)

build/%.o: src/%.cpp
	$(CXX) -c -std=c++14 $(CXXFLAGS) $< -o $@
