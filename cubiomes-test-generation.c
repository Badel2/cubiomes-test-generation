// clang-format -i cubiomes-test-generation.c -style="{BasedOnStyle: LLVM, IndentWidth: 4,
// ColumnLimit: 100}"
#include "cubiomes/generator.h"
#include "slime_seed_finder.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

int compare_biome_map(const int *map, const int *expected, size_t len, size_t *diff_index) {
    size_t *i = diff_index;
    for (*i = 0; *i < len; (*i)++) {
        // Skip unknown biomes
        if (expected[*i] == -1) {
            continue;
        }
        if (map[*i] < expected[*i]) {
            return -1;
        }
        if (map[*i] > expected[*i]) {
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
    }
    return NULL;
}

bool mc_version_before_1_15(int mc_version_int) { return mc_version_int < MC_1_15; }

void print_usage() {
    printf("Expected usage:\n");
    printf("\tcubiomes-test-generation --mc-version $MC_VERSION --input-zip $WORLD_ZIP_PATH\n");
}

int parse_args(int argc, const char **argv, int *mc_version_int, const char **world_zip_path) {
    if (argc != 5) {
        printf("Invalid arguments. ");
        print_usage();
        return 1;
    }

    if (strcmp(argv[1], "--mc-version")) {
        printf("Invalid arguments. ");
        print_usage();
        return 1;
    }

    if (strcmp(argv[3], "--input-zip")) {
        printf("Invalid arguments. ");
        print_usage();
        return 1;
    }

    const char *mc_version = argv[2];

    *mc_version_int = parse_cubiomes_mc_version(mc_version);
    if (*mc_version_int == -1) {
        printf("Invalid minecraft version: %s\n", mc_version);
        return 2;
    }

    *world_zip_path = argv[4];

    return 0;
}

// Expected usage:
// cubiomes-test-generation --mc-version $MC_VERSION --input-zip $WORLD_ZIP_PATH
int main(int argc, const char **argv) {
    int mc_version_int = -1;
    const char *world_zip_path = NULL;

    {
        int ret = parse_args(argc, argv, &mc_version_int, &world_zip_path);
        if (ret) {
            return ret;
        }
    }

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
    Map biome_map;
    {
        char *err = read_biome_map_from_mc_world(world_zip_path, mc_version, &biome_map);
        if (err) {
            printf("Error reading biome map from mc world: %s\n", err);
            free_error_msg(err);
            return 5;
        }
    }

    // Generate the same area using cubiomes

    // Initialize a stack of biome layers.
    LayerStack g;
    setupGenerator(&g, mc_version_int);

    // Extract the desired layer.
    Layer *layer;
    if (mc_version_before_1_15(mc_version_int)) {
        layer = &g.layers[L_VORONOI_1];
    } else {
        layer = &g.layers[L_OCEAN_MIX_4];
    }
    int64_t areaX = biome_map.x, areaZ = biome_map.z;
    uint64_t areaWidth = biome_map.w, areaHeight = biome_map.h;

    // Allocate a sufficient buffer for the biomes
    int *biomeIds = allocCache(layer, areaWidth, areaHeight);

    // Apply the seed only for the required layers and generate the area.
    setLayerSeed(layer, world_seed);
    printf("Generating world with seed %ld and size %ldx%ld (%ld bytes)\n", world_seed, areaWidth,
           areaHeight, areaWidth * areaHeight);
    genArea(layer, biomeIds, areaX, areaZ, areaWidth, areaHeight);

    // Compare biomeIds with biome_map
    size_t diff_index = 0;
    int different = compare_biome_map(biomeIds, biome_map.a, areaWidth * areaHeight, &diff_index);
    if (different) {
        // TODO: print index of first mismatch
        // let idx = (point.z - area.z) as usize * area.w as usize + (point.x -
        // area.x) as usize;
        int64_t diff_x = areaX + (diff_index % areaWidth);
        int64_t diff_z = areaZ + (diff_index / areaWidth);
        printf("Biome mismatch at (%ld, %ld),", diff_x, diff_z);
        printf(" expected %d got %d\n", biome_map.a[diff_index], biomeIds[diff_index]);
        print_array_diff(biomeIds, biome_map.a, areaWidth * areaHeight, diff_index);
        return 6;
    }

    printf("All biomes match\n");

    // Clean up.
    free(biomeIds);
    free_map(biome_map);

    return 0;
}
