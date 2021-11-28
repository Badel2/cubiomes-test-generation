# https://gist.github.com/rheum/3062b128f9ab31de7264

.PHONY: all cubiomes slime_seed_finder update-submodules

all: cubiomes slime_seed_finder cubiomes-test-generation

cubiomes: cubiomes/makefile
	@- make -C cubiomes

slime_seed_finder:
	cd slime_seed_finder && cargo build --release -p slime_seed_finder_c
	cp slime_seed_finder/slime_seed_finder_c/bindings.h slime_seed_finder.h
	cp slime_seed_finder/target/release/libslime_seed_finder.so libslime_seed_finder.so

parse_args:
	cd cubiomes-test-generation-parse-args && cargo build --release
	cp cubiomes-test-generation-parse-args/bindings.h cubiomes_test_generation_parse_args.h
	cp cubiomes-test-generation-parse-args/target/release/libcubiomes_test_generation_parse_args.so libcubiomes_test_generation_parse_args.so

# Linking is hard
# https://stackoverflow.com/a/23324703
ROOT_DIR:=$(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))

cubiomes-test-generation: slime_seed_finder cubiomes parse_args
	gcc -Wall -Werror -fwrapv -O2 cubiomes-test-generation.c -o cubiomes-test-generation -Wl,-rpath,${ROOT_DIR} -L. -Lcubiomes -lslime_seed_finder -lcubiomes -lcubiomes_test_generation_parse_args -lm
	echo "Compiled with rpath ${ROOT_DIR}"

%/Makefile:
	git submodule update --init --remote --recursive

update-submodules:
	git submodule update --init --remote --recursive

clean:
	git clean -f -d -x
	git submodule foreach --recursive make clean
