build:
	cmake -S . -B ./_build
	cmake --build ./_build --config Release

rerun:
	./_build/bin/demo

clean:
	rm -fr _build

run: clean build rerun

