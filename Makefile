# https://gist.github.com/rheum/3062b128f9ab31de7264

.PHONY: all cubiomes slime_seed_finder update-submodules

all: cubiomes slime_seed_finder cubiomes-test-generation

cubiomes: cubiomes/makefile
	@- make -C cubiomes

slime_seed_finder:
	cd slime_seed_finder && cargo build --release -p slime_seed_finder_c
	cp slime_seed_finder/slime_seed_finder_c/bindings.h slime_seed_finder.h
	cp slime_seed_finder/target/release/libslime_seed_finder.so libslime_seed_finder.so

# Linking is hard
# https://stackoverflow.com/a/23324703
ROOT_DIR:=$(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))

cubiomes-test-generation: slime_seed_finder cubiomes
	gcc -Wall -Werror -fwrapv -O2 cubiomes-test-generation.c -o cubiomes-test-generation -Wl,-rpath,${ROOT_DIR} -L. -Lcubiomes -lslime_seed_finder -lcubiomes -lm
	echo "Compiled with rpath ${ROOT_DIR}"

%/Makefile:
	git submodule update --init --remote --recursive

update-submodules:
	git submodule update --init --remote --recursive

clean:
	git clean -f -d -x
	git submodule foreach --recursive make clean
