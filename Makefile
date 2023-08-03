all: main

# debug = defined

main:
ifndef debug
	@echo "release"
	g++ src/*.cpp include/glad/glad.c \
	-o build/project.exe \
	-Wall -Wextra -O2 -std=c++17 -mwindows \
	-I include/ \
	-L lib/ \
	-lglfw3 -lopengl32 -lgdi32 -lwinmm
else
	@echo "debug"
	g++ src/*.cpp include/glad/glad.c \
	-o build/project.exe \
	-g -Og -Wall -Wextra -O2 -std=c++17 \
	-I include/ \
	-L lib/ \
	-lglfw3 -lopengl32 -lgdi32 -lwinmm

endif

run: main
	./build/project.exe

.PHONY: all run