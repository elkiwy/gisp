SRC := src
OBJ := obj
BUILD := build
OBJPROF := obj_profiling

CC := gcc

SOURCES := $(wildcard $(SRC)/*.c)
OBJECTS := $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SOURCES))
OBJECTSPROF := $(patsubst $(SRC)/%.c, $(OBJPROF)/%.o, $(SOURCES))
FLAGS = -g -Og -Wshadow -Wextra -Werror=implicit-int -Werror=incompatible-pointer-types -Werror=int-conversion -fsanitize=address -fsanitize=undefined



build/main: gispCore $(OBJECTS) 
	mkdir -p $(BUILD)
	mkdir -p $(OBJ)
	$(CC) $(OBJECTS) $(FLAGS) -lcairo -lm -ldl -o $@ 

gispCore:
	rm -f $(SRC)/gispCore.h
	echo '#ifndef _GISPCORE_H' >> $(SRC)/gispCore.h
	echo '#define _GISPCORE_H' >> $(SRC)/gispCore.h
	for i in src/gisp-core/*; do \
		xxd -i $$i >> $(SRC)/gispCore.h; \
	done
	echo '#endif /* _GISPCORE_H */' >> $(SRC)/gispCore.h



$(OBJ)/%.o: $(SRC)/%.c
	$(CC) $(FLAGS) -c $< -I$(SRC) -I/usr/local/include/cairo -o $@

run: build/main
	./build/main

docu:
	docu -i src -m HTML -s Docs.css -o Docs.html
	docu -i src -m DEBUG




install: build/main
	sudo cp build/main /usr/local/bin/gisp

clean:
	rm -r build && rm -r $(OBJ) && mkdir build && mkdir $(OBJ)


test: clean build/main
	./build/main src/test.gisp --time --memory
sketch: clean build/main
	build/main ~/Documents/artworks/SKETCH/sketch.gisp --time

valgrind-sketch: clean build/main
	valgrind --leak-check=yes build/main ~/Documents/artworks/SKETCH/sketch.gisp --time	
valgrind-test: clean build/main
	valgrind --leak-check=yes build/main ~/Documents/gisp/src/test.gisp --time	
