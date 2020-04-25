SRC := src
OBJ := obj
OBJPROF := obj_profiling

CC := gcc

SOURCES := $(wildcard $(SRC)/*.c)
OBJECTS := $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SOURCES))
OBJECTSPROF := $(patsubst $(SRC)/%.c, $(OBJPROF)/%.o, $(SOURCES))
FLAGS = -g -Og -Wshadow -Wextra -Werror=implicit-int -Werror=incompatible-pointer-types -Werror=int-conversion -fsanitize=address -fsanitize=undefined



build/main: $(OBJECTS)
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
	./build/main_profiling src/test.gisp
	xcrun llvm-profdata merge -output=test.profdata -instr default.profraw
	xcrun llvm-profdata show -all-functions -counts -ic-targets  test.profdata > test.log
	xcrun llvm-cov show -format=html -instr-profile=test.profdata build/main_profiling > coverage.html


profile-text: build/main_profiling
	./build/main_profiling src/test.gisp
	xcrun llvm-profdata merge -output=test.profdata -instr default.profraw
	xcrun llvm-profdata show -all-functions -counts -ic-targets  test.profdata > test.log
	xcrun llvm-cov show -format=text -instr-profile=test.profdata build/main_profiling > coverage.txt




