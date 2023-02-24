all: clean install cmakes build

cmakes:
	cmake -S . -B build -G Ninja -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release

build:
	cmake --build build --config Release -j 4

install:
	conan install . -of build -b missing -u

clean:
	rm -rf build
