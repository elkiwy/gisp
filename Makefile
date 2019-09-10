SRC := src
OBJ := obj

CC := gcc

SOURCES := $(wildcard $(SRC)/*.c)
OBJECTS := $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SOURCES))

build/main: $(OBJECTS)
	$(CC) $^ -g -Wall -L/usr/local/lib/ -lcairo -o $@

$(OBJ)/%.o: $(SRC)/%.c
	$(CC) -g -Wall -c $< -I$(SRC) -I/usr/local/include/cairo -o $@

run: build/main
	./build/main

test: build/main
	sed -e '/^;/d' src/test.lisp | ./build/main

debug: rebuild
	gdb build/main

debug-mi: 
	gdb -i=mi build/main

docker-build:
	docker build -t elkiwy/gdb .

docker-debug: 
	docker run -it --rm --cap-add=SYS_PTRACE --security-opt seccomp=unconfined elkiwy/gdb	

clean:
	rm -r build && rm -r $(OBJ) && mkdir build && mkdir $(OBJ)

rebuild: clean build/main

