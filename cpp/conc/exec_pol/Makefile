build:
	cmake -S . -B ./_build
	cmake --build ./_build --config Release

rerun:
	./_build/bin/demo

clean:
	rm -fr _build

run: clean build rerun

clang_run:
	clang++ -std=c++23 main.cpp -o3 -o demo_clang
	./demo_clang

gcc_run: 
	gcc main.cpp -lstdc++ -std=c++23 -o3 -o demo_gcc
	./demo_gcc
