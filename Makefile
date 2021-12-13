all: build

cmake:
	dir="build"

	if [ ! -d $dir ]; then
		mkdir -p $dir

		conan install . -if $dir -b missing -u

		cmake -S . -B $dir -G Ninja -DCMAKE_EXPORT_COMPILE_COMMANDS=YES
	fi

build:
	cmake --build build --config Release -j 4

install:
	conan install . -if build -b missing -u

clean:
	rm -rf build