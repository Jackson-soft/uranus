all: clean install cmakes build

cmakes:
	cmake -S . -B build -G Ninja -DCMAKE_EXPORT_COMPILE_COMMANDS=YES

build:
	cmake --build build --config Release -j 4

install:
	conan install . -if build -b missing -u

clean:
	rm -rf build
