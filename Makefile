build/main: src/main.c
	gcc -g -Wall src/main.c -o build/main

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

docker-debug: rebuild
	docker run -it --rm --cap-add=SYS_PTRACE --security-opt seccomp=unconfined elkiwy/gdb	

clean:
	rm -r build && mkdir build

rebuild: clean build/main

