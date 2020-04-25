SRC := src
OBJ := obj
BUILD := build
OBJPROF := obj_profiling

CC := gcc

SOURCES := $(wildcard $(SRC)/*.c)
OBJECTS := $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SOURCES))
OBJECTSPROF := $(patsubst $(SRC)/%.c, $(OBJPROF)/%.o, $(SOURCES))
FLAGS = -g -Og -Wshadow -Wextra -Werror=implicit-int -Werror=incompatible-pointer-types -Werror=int-conversion -fsanitize=address -fsanitize=undefined



build/main: $(OBJECTS)
	mkdir -p $(BUILD)
	mkdir -p $(OBJ)
	ld -r -b binary -o obj/gisp_core.o src/core.gisp
	ld -r -b binary -o obj/gisp_noise.o src/simplex-noise.gisp
	$(CC) $^ obj/gisp_core.o obj/gisp_noise.o $(FLAGS) -lcairo -lm -ldl -o $@ 

$(OBJ)/%.o: $(SRC)/%.c
	$(CC) $(FLAGS) -c $< -I$(SRC) -I/usr/local/include/cairo -o $@

run: build/main
	./build/main

docu:
	docu -i src -m HTML -s Docs.css -o Docs.html
	docu -i src -m DEBUG

test: build/main
	./build/main src/test.gisp

fulltest: build/main
	./build/main ~/Documents/artworks/sketch-simplex.gisp

simplextest: build/main
	./build/main src/simplex-noise-profile.gisp

install: build/main
	sudo cp build/main /usr/local/bin/gisp

clean:
	rm -r build && rm -r $(OBJ) && mkdir build && mkdir $(OBJ)



