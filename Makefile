build/main: src/main.c
	gcc -g -Wall -std=c11 src/main.c -o build/main

run: build/main
	./build/main

test: build/main
	echo "(cons 1 2)" - | ./build/main

debug: build/main
	gdb build/main

docker-build:
	docker build -t elkiwy/gdb .

docker-debug:
	docker run -it --rm --cap-add=SYS_PTRACE --security-opt seccomp=unconfined elkiwy/gdb	

clean:
	rm build/main


