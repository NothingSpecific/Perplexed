all:
	./build.sh --build=all
release:
	./build.sh --build=release
install:
	./build.sh --build=install
debug:
	./build.sh --build=debug
tsan:
	./build.sh --build=tsan
valgrind:
	./build.sh --build=valgrind
clean:
	./build.sh --build=clean
