all: main

flags.debug := -Wall -Wextra -g -Og
flags.release := -Wall -Wextra -O2 -mwindows

extra_flags := ${flags.${build}}

main:
	g++ src/*.cpp include/glad/glad.c \
	-o build/learnopengl.exe -std=c++17 \
	$(extra_flags) \
	-I include/ \
	-L lib/ \
	-lglfw3 -lassimp -lopengl32 -lgdi32 -lwinmm

run: main
	./build/learnopengl.exe

.PHONY: all run