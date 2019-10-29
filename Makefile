SRC := src
OBJ := obj
OBJPROF := obj_profiling

CC := gcc

SOURCES := $(wildcard $(SRC)/*.c)
OBJECTS := $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SOURCES))
OBJECTSPROF := $(patsubst $(SRC)/%.c, $(OBJPROF)/%.o, $(SOURCES))

build/main: $(OBJECTS)
	$(CC) $^ -Wall -L/usr/local/lib/ -lcairo -lm -ldl -o $@

$(OBJ)/%.o: $(SRC)/%.c
	$(CC) -Wall -c $< -I$(SRC) -I/usr/local/include/cairo -o $@

run: build/main
	./build/main

test: build/main
	./build/main src/test.lisp

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



build/main_profiling: $(OBJECTSPROF)
	clang $^ -fprofile-instr-generate -fcoverage-mapping -L/usr/local/lib/ -lcairo -lm -ldl -o $@

$(OBJPROF)/%.o: $(SRC)/%.c
	clang -fprofile-instr-generate -fcoverage-mapping -c $< -I$(SRC) -I/usr/local/include/cairo -o $@

profile-html: build/main_profiling
	./build/main_profiling src/test.lisp
	xcrun llvm-profdata merge -output=test.profdata -instr default.profraw
	xcrun llvm-profdata show -all-functions -counts -ic-targets  test.profdata > test.log
	xcrun llvm-cov show -format=html -instr-profile=test.profdata build/main_profiling > coverage.html


profile-text: build/main_profiling
	./build/main_profiling src/test.lisp
	xcrun llvm-profdata merge -output=test.profdata -instr default.profraw
	xcrun llvm-profdata show -all-functions -counts -ic-targets  test.profdata > test.log
	xcrun llvm-cov show -format=text -instr-profile=test.profdata build/main_profiling > coverage.txt




