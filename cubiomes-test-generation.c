// clang-format -i cubiomes-test-generation.c -style="{BasedOnStyle: LLVM, IndentWidth: 4,
// ColumnLimit: 100}"
#include "cubiomes/generator.h"
#include "cubiomes/util.h"
#include "cubiomes_test_generation_parse_args.h"
#include "slime_seed_finder.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

// Since 1.18, biomes have no biome id, so any library can use whatever id they want.
// Let's normalize to cubiomes biome id.
// ssf biome id:
// https://github.com/owengage/fastnbt/blob/master/fastanvil/src/biome.rs
// cubiomes biome id:
// https://github.com/Cubitect/cubiomes/blob/master/layers.h
int ssf_biome_id_to_cubiomes_biome_id(const int ssf_biome_id) {
    switch (ssf_biome_id) {
    case 174:
        return dripstone_caves;
    case 175:
        return frozen_peaks;
    case 176:
        return grove;
    case 177:
        return jagged_peaks;
    case 178:
        return lush_caves;
    case 179:
        return meadow;
    case 180:
        return nether_wastes;
    case 181:
        return old_growth_birch_forest;
    case 182:
        return old_growth_pine_taiga;
    case 183:
        return old_growth_spruce_taiga;
    case 184:
        return snowy_plains;
    case 185:
        return snowy_slopes;
    case 186:
        return sparse_jungle;
    case 187:
        return stony_peaks;
    case 188:
        return stony_shore;
    case 189:
        return windswept_forest;
    case 190:
        return windswept_gravelly_hills;
    case 191:
        return windswept_hills;
    case 192:
        return windswept_savanna;
    case 193:
        return wooded_badlands;
    }
    return ssf_biome_id;
}

int compare_biome_map(const int *map, const int *expected, size_t len, size_t *diff_index) {
    size_t *i = diff_index;
    for (*i = 0; *i < len; (*i)++) {
        // Skip unknown biomes
        if (expected[*i] == -1) {
            continue;
        }
        // TODO: There is a bug in 1.18 where chunks that are not fully
        // generated have biome: Plains So until that's fixed in
        // slime_seed_finder, ignore plains
        if (expected[*i] == 1) {
            continue;
        }
        // Conversion not needed, has been done previously
        // int e = ssf_biome_id_to_cubiomes_biome_id(expected[*i]);
        int e = expected[*i];
        if (map[*i] == e) {
            continue;
        }
        if (map[*i] < e) {
            return -1;
        }
        if (map[*i] > e) {
            return 1;
        }
    }

    return 0;
}

void print_array_diff(const int *a, const int *b, size_t len, size_t diff_index) {
    // 20 elements of context on each side of the diff_index
    size_t context = 20;
    size_t end = diff_index + context + 1;
    if (end >= len) {
        end = len - 1;
    }
    size_t start = end;
    if (start < 2 * context) {
        start = 0;
    } else {
        start -= 2 * context;
    }

    printf("left:  [");
    for (size_t i = start; i < end; i++) {
        if (i != start) {
            printf(", ");
        }
        printf("%d", a[i]);
    }
    printf("]\n");
    printf("right: [");
    for (size_t i = start; i < end; i++) {
        if (i != start) {
            printf(", ");
        }
        printf("%d", b[i]);
    }
    printf("]\n");
}

int parse_cubiomes_mc_version(const char *mc_version) {
    // Nope, no way I'm implementing a parser in C
    // *5 minutes later*
    // Oh, fuck

    bool eat_small_int(const char **c, int *out) {
        const char *orig_c = *c;
        int x = 0;
        if (!(**c >= '0' && **c <= '9')) {
            return false;
        }

        x += **c - '0';
        *c = *c + 1;
        int next_digit = 0;
        if (!eat_small_int(c, &next_digit)) {
            *out = x;
            return true;
        } else {
            x = x * 10 + next_digit;
            if (x >= 50000) {
                *c = orig_c;
                return false;
            } else {
                *out = x;
                return true;
            }
        }
    }

    bool eat_chr(const char **c, char x) {
        if (**c == x) {
            *c = *c + 1;
            return true;
        } else {
            return false;
        }
    }

    bool is_eof(const char *c) { return *c == '\0'; }

    int hi = 0;
    int mid = 0;
    int lo = 0;
    if (!eat_small_int(&mc_version, &hi)) {
        return -1;
    }
    if (!eat_chr(&mc_version, '.')) {
        return -1;
    }
    if (!eat_small_int(&mc_version, &mid)) {
        return -1;
    }
    if (!is_eof(mc_version)) {
        if (!eat_chr(&mc_version, '.')) {
            return -1;
        }
        if (!eat_small_int(&mc_version, &lo)) {
            return -1;
        }
        if (!is_eof(mc_version)) {
            return -1;
        }
    }

    if (hi == 1) {
        switch (mid) {
        case 0:
            return MC_1_0;
        case 1:
            return MC_1_1;
        case 2:
            return MC_1_2;
        case 3:
            return MC_1_3;
        case 4:
            return MC_1_4;
        case 5:
            return MC_1_5;
        case 6:
            return MC_1_6;
        case 7:
            return MC_1_7;
        case 8:
            return MC_1_8;
        case 9:
            return MC_1_9;
        case 10:
            return MC_1_10;
        case 11:
            return MC_1_11;
        case 12:
            return MC_1_12;
        case 13:
            return MC_1_13;
        case 14:
            return MC_1_14;
        case 15:
            return MC_1_15;
        case 16:
            return MC_1_16;
        case 17:
            return MC_1_17;
        case 18:
            return MC_1_18;
        }
    }

    return -1;
}

const char *mc_version_to_string(int mc_version_int) {
    switch (mc_version_int) {
    case MC_1_0:
        return "1.0";
    case MC_1_1:
        return "1.1";
    case MC_1_2:
        return "1.2";
    case MC_1_3:
        return "1.3";
    case MC_1_4:
        return "1.4";
    case MC_1_5:
        return "1.5";
    case MC_1_6:
        return "1.6";
    case MC_1_7:
        return "1.7";
    case MC_1_8:
        return "1.8";
    case MC_1_9:
        return "1.9";
    case MC_1_10:
        return "1.10";
    case MC_1_11:
        return "1.11";
    case MC_1_12:
        return "1.12";
    case MC_1_13:
        return "1.13";
    case MC_1_14:
        return "1.14";
    case MC_1_15:
        return "1.15";
    case MC_1_16:
        return "1.16";
    case MC_1_17:
        return "1.17";
    case MC_1_18:
        return "1.18";
    }
    return NULL;
}

// Expected usage:
// cubiomes-test-generation --mc-version $MC_VERSION --input-zip $WORLD_ZIP_PATH --save-img
int main(int argc, const char **argv) {
    Args args;

    {
        int ret = parse_args(argc, argv, &args);
        if (ret) {
            return ret;
        }
    }

    int mc_version_int = parse_cubiomes_mc_version(args.mc_version);
    if (mc_version_int == -1) {
        printf("Invalid minecraft version: %s\n", args.mc_version);
        return 2;
    }
    const char *world_zip_path = args.input_zip;

    const char *mc_version = mc_version_to_string(mc_version_int);
    if (mc_version == NULL) {
        printf("Error converting mc_version to string\n");
        return 3;
    }

    printf("Reading zip file %s\n", world_zip_path);

    // Read world seed from zip file
    int64_t world_seed = 0;
    {
        char *err = read_seed_from_mc_world(world_zip_path, mc_version, &world_seed);
        if (err) {
            printf("Error reading seed from mc world: %s\n", err);
            free_error_msg(err);
            return 4;
        }
    }

    printf("Reading world with seed %ld and version %s\n", world_seed, mc_version);

    // Read biome map from zip file
    Map3D biome_map;
    {
        char *err = read_biome_map_from_mc_world(world_zip_path, mc_version, &biome_map);
        if (err) {
            printf("Error reading biome map from mc world: %s\n", err);
            free_error_msg(err);
            return 5;
        }
    }

    // Slice biome map if y_level arg is set
    // Store original y and sy args, they are needed to free the memory later
    Map3D original_biome_map = biome_map;
    if (args.y_level.is_some) {
	int64_t y_offset = args.y_level.value - biome_map.y;
	if (y_offset >= biome_map.sy || y_offset < 0) {
		printf("Error: y_level is outside of bounds. min_y_level=%ld, max_y_level=%ld\n", biome_map.y, biome_map.y + biome_map.sy - 1);
		return 14;
	}
	memcpy(&biome_map.a[0], &biome_map.a[(biome_map.sx * biome_map.sz) * y_offset], biome_map.sx * biome_map.sz);
	biome_map.y = args.y_level.value;
	biome_map.sy = 1;
    }

    // Convert ssf biome id into cubiomes biome id
    for (size_t i = 0; i < biome_map.sx * biome_map.sy * biome_map.sz; i++) {
        biome_map.a[i] = ssf_biome_id_to_cubiomes_biome_id(biome_map.a[i]);
    }

    if (args.save_img) {
        char *err = draw_map3d_image_to_file(&biome_map, "biome_map_c_from_zip.png");
        if (err) {
            printf("Error saving biome map to file: %s\n", err);
            free_error_msg(err);
            return 6;
        }
    }
    // Generate the same area using cubiomes

    Generator g;
    setupGenerator(&g, mc_version_int, 0);

    applySeed(&g, 0, world_seed);

    Range r;
    r.scale = 4;
    r.x = biome_map.x;
    r.y = biome_map.y;
    r.z = biome_map.z;
    r.sx = biome_map.sx;
    r.sy = biome_map.sy;
    r.sz = biome_map.sz;
    // Set the vertical range as a plane near sea level at scale 1:4.
    // r.y = 15;

    // Allocate a sufficient buffer for the biomes
    int *biomeIds = allocCache(&g, r);

    printf("Generating world with seed %ld and size %dx%dx%d (%ld bytes)\n", world_seed, r.sx, r.sy,
           r.sz, (uint64_t)r.sx * (uint64_t)r.sy * (uint64_t)r.sz);
    genBiomes(&g, biomeIds, r);

    if (args.save_img) {
        Map3D cubiomes_map = biome_map;
        cubiomes_map.a = biomeIds;
        char *err = draw_map3d_image_to_file(&cubiomes_map, "biome_map_c_from_cubiomes.png");
        if (err) {
            printf("Error saving biome map to file: %s\n", err);
            free_error_msg(err);
            return 7;
        }
    }

    // Compare biomeIds with biome_map
    size_t diff_index = 0;
    int different = compare_biome_map(biomeIds, biome_map.a, r.sx * r.sy * r.sz, &diff_index);
    if (different) {
        // TODO: print index of first mismatch
        // let idx = (point.z - area.z) as usize * area.w as usize + (point.x -
        // area.x) as usize;
        int64_t diff_x = r.x + (diff_index % r.sx);
        int64_t diff_z = r.z + ((diff_index / r.sx) % r.sz);
        int64_t diff_y = r.y + (diff_index / (r.sx * r.sz));
        printf("Biome mismatch at %ld (%ld, %ld, %ld),", diff_index, diff_x, diff_y, diff_z);
        printf(" expected %d got %d\n", biome_map.a[diff_index], biomeIds[diff_index]);
        print_array_diff(biomeIds, biome_map.a, r.sx * r.sy * r.sz, diff_index);
        return 8;
    }

    printf("All biomes match\n");

    // Clean up.
    free(biomeIds);
    free_map(original_biome_map);
    free_args(args);

    return 0;
}
