project_name := learnopengl
output := $(project_name).exe

all: $(output)

# compile variables
cxx := g++
std := -std=c++17
warnings := -Wall -Wextra -pedantic -Wcast-align -Wcast-qual \
-Wctor-dtor-privacy -Wdisabled-optimization -Winit-self -Wlogical-op \
-Wmissing-declarations -Wmissing-include-dirs -Wnoexcept -Woverloaded-virtual \
-Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo \
-Wstrict-null-sentinel -Wswitch-default -Wundef -Wno-unused

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
	@$(cxx) $(filter-out $(libs),$^) -o $@ $(std) $(warnings) $(extra_flags) -L lib/ -lglfw3 -lassimp -lopengl32 -lgdi32 -lwinmm -lglad
	@echo $@

-include $(depends)

# build all src/%.cpp to build/%.o
build/%.o: src/%.cpp Makefile
	@$(cxx) -c $< -o $@ $(std) $(warnings) $(extra_flags) -MMD -MP -I src/include/ -isystem include/
	@echo "$< > $@"

# copy all lib/%.dll to /%.dll
%.dll: lib/%.dll
	@cp $^ $@
	@echo "cp $^ > $@"

run: $(output)
	./$(output)

clean:
	@rm -f $(output)
	@echo "rm output"
	@rm -f $(objects)
	@echo "rm objects"
	@rm -f $(depends)
	@echo "rm depends"
	@rm -f $(libs)
	@echo "rm libs"

.PHONY: all run clean