project_name := learnopengl
output := $(project_name).exe

all: $(output)

# compile variables
cc := g++
std := -std=c++17
warnings := -Wall -Wextra

# build flags
build_flags.debug := -O0 -ggdb3
build_flags.release := -O3 -mwindows
extra_flags := ${build_flags.${build}}

# get all src/*.cpp
sources := $(wildcard src/*.cpp)
# create a list of build/%.o based on sources list
objects := $(patsubst src/%.cpp, build/%.o, $(sources))
# create a list of build/%.d based on sources list
depends := $(patsubst src/%.cpp, build/%.d, $(sources))

# get all lib/*.dll
shared_libs := $(wildcard lib/*.dll)
# create a list of /%.dll based on shared_libs list
libs := $(patsubst lib/%.dll, %.dll, $(shared_libs))

# link all compiled objects
$(output): $(objects) $(libs)
	$(cc) $(filter-out $(libs),$^) -o $@ $(std) $(warnings) $(extra_flags) -L lib/ -lglfw3 -lassimp -lopengl32 -lgdi32 -lwinmm -lglad

-include $(depends)

# build all src/%.cpp to build/%.o
build/%.o: src/%.cpp Makefile
	$(cc) -c $< -o $@ $(std) $(warnings) $(extra_flags) -MMD -MP -I include/

# copy all lib/%.dll to /%.dll
%.dll: lib/%.dll
	cp $^ $@

run: $(output)
	./$(output)

clean:
	rm -f $(output)
	rm -f $(objects)
	rm -f $(depends)
	rm -f $(libs)

.PHONY: all run clean