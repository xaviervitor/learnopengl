all: main

build = release # fast | release | debug

ifeq ($(build), fast)
	additional_flags = 
else ifeq ($(build), release)
	additional_flags = -Wall -Wextra -O2 -mwindows
else ifeq ($(build), debug)
	additional_flags = -Wall -Wextra -g -Og
endif

main:
	@echo $(build) "build"
	
	g++ src/*.cpp include/glad/glad.c \
	-o build/project.exe -std=c++17 \
	$(additional_flags) \
	-I include/ \
	-L lib/ \
	-lglfw3 -lopengl32 -lgdi32 -lwinmm

run: main
	./build/project.exe

.PHONY: all run