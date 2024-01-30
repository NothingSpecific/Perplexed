build:
	./build.sh
release:
	./build.sh --build=release
install:
	./build.sh --build=install
debug:
	./build.sh --build=debug
tsan:
	./build.sh --build=tsan
clean:
	./build.sh --build=clean
