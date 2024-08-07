all: clean install cmakes build

cmakes:
	cmake -S . -B build -G Ninja -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_POLICY_DEFAULT_CMP0091=NEW -DCMAKE_BUILD_TYPE=Debug

build:
	cmake --build build --config Release -j 4

install:
	conan install conanfile.txt -b=missing -u

debug:
	conan install conanfile.txt -b=missing -u -s build_type=Debug
clean:
	rm -rf build
