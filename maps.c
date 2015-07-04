#include "ngine.h"

//initialize a map
void load_map(Game* game, TMX_map* map){
    fprintf(fpLog, "Loading map...\n");
    int i, j;

    FILE* fp;
    char dir[128] = "";
    //get size of map
    sprintf(dir, "res/maps/%s/map.map", game->active_map);
    fp = fopen(dir, "r");
    if(game->debug && fp == NULL){
        fprintf(fpLog, "File not opened map.map\n");
    }

    get_dims(fp, &map->w, &map->l);

    //allocate memory for map
    map->tmx = (int**) malloc(map->l * sizeof(int));
    for(i = 0; i < map->l; ++i){
        map->tmx[i] = (int*) malloc(map->w * sizeof(int));
        for(j = 0; j < map->w; ++j){
            //scan in map to locaiton
            fscanf(fp, "%d", &map->tmx[i][j]);
        }
    }
    fclose(fp);

    //get tilemap
    sprintf(dir, "res/maps/%s/tilemap.bmp", game->active_map);
    if ((map->tilemap = load_bitmap(dir, NULL)) == NULL){
        fprintf(fpLog, "Failed to load tilemap\n");
    }

    map->blocker_values = NULL;
    sprintf(dir, "res/maps/%s/settings.map", game->active_map);
    int temp;
    int cR, cG, cB;

    fp = fopen(dir, "r");
    if(game->debug && fp == NULL){
        fprintf(fpLog, "File not opened settings.map\n");
    }

    //get and set default background color
    fscanf(fp, "%d %d %d", &cR, &cG, &cB);
    map->bgCol = makecol(cR, cG, cB);

    i = 0;
    while(fscanf(fp, "%d", &temp) != EOF){
        map->blocker_values = (int*) realloc(map->blocker_values,
                                             (++i) * sizeof(int));
        map->blocker_values[i-1] = temp;
    }
    map->blocker_val_count = i;
    //generate background
    map->map = NULL;
    gen_map(map);
}END_OF_FUNCTION(load_map);

//generate background to map
void gen_map(TMX_map* map){
    destroy_bitmap(map->map);
    int i, j;
    map->map = create_bitmap(map->w * tile_size, map->l * tile_size);
    //set bg color as fushia
    clear_to_color(map->map, map->bgCol);
    //loop through map values and map to background
    for(i = 0; i < map->l; ++i){
        for(j = 0; j < map->w; ++j){
            masked_blit(map->tilemap, map->map,
                        map->tmx[i][j] % 8 * tile_size,
                        map->tmx[i][j] / 8 * tile_size, j * tile_size,
                        i * tile_size, tile_size, tile_size);
        }
    }
    blockers_generate(map->blocker_values, map);
}END_OF_FUNCTION(gen_map);

