SRC := src
OBJ := obj
BUILD := build
OBJPROF := obj_profiling

CC := gcc-10

SOURCES := $(wildcard $(SRC)/*.c)
OBJECTS := $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SOURCES))
OBJECTSPROF := $(patsubst $(SRC)/%.c, $(OBJPROF)/%.o, $(SOURCES))

WARNS = -Wshadow -Wextra -Werror=implicit-int -Werror=incompatible-pointer-types -Werror=int-conversion
OPENMP=-fopenmp

FLAGS = -I/usr/local/include -g -Og $(WARNS)  #-fsanitize=address -fsanitize=undefined

LIBCAIRO = -lcairo -lm -ldl
LIBTRACING = -ltracing
LIBOPENCL = -framework OpenCL -arch x86_64 -DUNIX -DDEBUG -DMAC
LIBS = -L/usr/local/lib $(LIBCAIRO) $(LIBTRACING) $(LIBOPENCL)


build/main: gispCore $(OBJECTS) 
	mkdir -p $(BUILD)
	mkdir -p $(OBJ)
	$(CC) $(OBJECTS) $(FLAGS) $(OPENMP) $(LIBS) -o $@

gispCore:
	rm -f $(SRC)/gispCore.h
	echo '#ifndef _GISPCORE_H' >> $(SRC)/gispCore.h
	echo '#define _GISPCORE_H' >> $(SRC)/gispCore.h
	for i in src/gisp-core/*; do \
		xxd -i $$i >> $(SRC)/gispCore.h; \
	done
	echo '#endif /* _GISPCORE_H */' >> $(SRC)/gispCore.h



$(OBJ)/%.o: $(SRC)/%.c
	$(CC) $(FLAGS) $(OPENMP) -c $< -I$(SRC) -I/usr/local/include/cairo -o $@

run: build/main
	./build/main

docu:
	docu -i src -m HTML -s Docs.css -o Docs.html
	docu -i src -m DEBUG




install: build/main
	sudo cp build/main /usr/local/bin/gisp

clean:
	rm -r build && rm -r $(OBJ) && mkdir build && mkdir $(OBJ)


test3D: clean build/main
	./build/main src/test_raytracing_cornellbox.gisp --time --memory
test: clean build/main
	./build/main src/test.gisp --time --memory
sketch: clean build/main
	build/main ~/Documents/artworks/SKETCH/sketch.gisp --time

valgrind-sketch: clean build/main
	valgrind --leak-check=yes build/main ~/Documents/artworks/SKETCH/sketch.gisp --time	
valgrind-test: clean build/main
	valgrind --leak-check=yes build/main ~/Documents/gisp/src/test.gisp --time	
