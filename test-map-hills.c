#include "cubiomes/generator.h"
#include "slime_seed_finder.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char ** argv) {
    int mc_version_int = MC_1_14;
    int64_t world_seed = 797383349100663716;

    // Initialize a stack of biome layers.
    LayerStack g;
    setupGenerator(&g, mc_version_int);

    // Extract the desired layer.
    Layer *layer;
    layer = &g.layers[L_HILLS_64];

    int64_t areaX = 11, areaZ = -6;
    uint64_t areaWidth = 3, areaHeight = 5;

    // Allocate a sufficient buffer for the biomes
    int *biomeIds = allocCache(layer, areaWidth, areaHeight);

    // Apply the seed only for the required layers and generate the area.
    setLayerSeed(layer, world_seed);
    printf("Generating world with seed %ld and size %ldx%ld (%ld bytes)\n", world_seed, areaWidth,
           areaHeight, areaWidth * areaHeight);
    genArea(layer, biomeIds, areaX, areaZ, areaWidth, areaHeight);

    for (int i=0; i<areaHeight; i++) {
        for (int j=0; j<areaWidth; j++) {
            printf("%d, ", biomeIds[i*areaWidth + j]);
	}
        printf("\n");
    }

    return 0;
}
