all: prepare build

prepare:
	cmake -B staging

clean:
	rm -rf staging

build:
	cmake --build staging

run: run-sdl

run-sdl:
	staging/./kaji-sdl

run-cli:
	staging/./kaji-cli	