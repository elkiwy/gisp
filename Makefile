build/main: src/main.c
	gcc -g -Wall src/main.c -o build/main

run: build/main
	./build/main

test: build/main
	echo "(cons 1 2)" - | ./build/main

debug: 
	gdb build/main

debug-mi: 
	gdb -i=mi build/main

docker-build:
	docker build -t elkiwy/gdb .

docker-debug:
	docker run -it --rm --cap-add=SYS_PTRACE --security-opt seccomp=unconfined elkiwy/gdb	

clean:
	rm -r build && mkdir build


