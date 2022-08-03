
all: build

build: test.elf 

test.elf: aie.mlir test.cpp
	aiecc.py -v --aie-generate-xaiev2 --sysroot= $^ -I${ACDC}/runtime_lib ${ACDC}/runtime_lib/test_library.cpp -o $@

clean:
	rm -rf *.elf acdc_project *.o
