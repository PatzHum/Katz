#include "ngine.h"


void blockers_generate(int* wall_v, TMX_map* src_map){
    int i, j, k;    //counters
    int wall_count = src_map->blocker_val_count;
    //get size of map and scan it in from a text file
    int x, y;
    x = src_map->w;
    y = src_map->l;

    int map[x][y];
    for(i = 0; i < y; ++i){
        for(j = 0; j < x; ++j){
            map[i][j] = src_map->tmx[i][j];
        }
    }

    //Set outer bounds of the walls, always constant
    src_map->blockers_count = 0;
    src_map->blockers = NULL;

    add_blocker(src_map, -100, -100, x * tile_size + 100, 0);
    add_blocker(src_map, -100, 0, 0, y * tile_size + 100);
    add_blocker(src_map, 0, y * tile_size, x * tile_size + 100, y *
                tile_size + 100);
    add_blocker(src_map, x * tile_size, 0, x * tile_size + 100, y * tile_size);


    //loop through text file looking for rectangles of blockers
    int keep_running = 1;
    int run;
    int rect[4];    //array to store rectangle values to
    while(keep_running){
        run = 1;
        //reset rect to 0
        memcpy(rect, (int[4]){0, 0, 0, 0}, 4 * sizeof(int));
        /*loop through looking for top left corner of rectangle
        on finding it save location and loop to top right corner
        and save that location, now we have 3/4 vals for rect*/
        for(i = 0; i < y && run; ++i){
            for(j = 0; j < x && run; ++j){
                if (is_wall(map[i][j], wall_v, wall_count)){
                    rect[0] = j;
                    rect[1] = i;
                    for(k = j; k <= x; ++k){
                        if (!is_wall(map[i][k], wall_v, wall_count) ||
                            k == x){
                            rect[2] = k;
                            break;
                        }
                    }
                    run = 0;
                }
            }
        }
        run = 1;
        /*loop vertically down map and through each line until:
            A) one tile isn't a wall AND/OR
            B) there are walls left and right of the line
                --note (B) is purely for optimization purposes*/
        for(i = rect[1]; i <= y && run; ++i){
            //(B)
            if (rect[0] > 0 && rect[2] < x){
                if (is_wall(map[i][rect[0]-1],
                    wall_v, wall_count) && is_wall(map[i][rect[2]],
                    wall_v, wall_count)){
                    rect[3] = i-1;
                    run = 0;
                }
            }
            //(A)
            for(j = rect[0]; j < rect[2] && run; ++j){
                if (!is_wall(map[i][j], wall_v, wall_count)){
                    rect[3] = i-1;
                    run = 0;
                }
            }
            /*set all looped values to -1 so there is no overlap
            on next iteration*/
            for(j = rect[0]; j < rect[2] && run; ++j){
                map[i][j] = -1;
            }
        }
        //check if we looped through entire map without finding anything
        if (rect[2] == 0 && rect[3] == 0){
            keep_running = 0;   //stop running and exit function
        }else{
            add_blocker(src_map, rect[0] * tile_size ,
                        rect[1] * tile_size,
                        rect[2] * tile_size,
                        (rect[3] + 1) * tile_size);
        }
    }
}END_OF_FUNCTION(blockers_generate);

int add_blocker(TMX_map* map, int x1, int y1, int x2, int y2){
    //resize blockers output and save to memory
    map->blockers = (int**) realloc(map->blockers,
        (map->blockers_count+1)* sizeof(int*));
    (map->blockers)[map->blockers_count] = (int*) malloc(4 * sizeof(int));

    memcpy(map->blockers[map->blockers_count++],
           (int[4]){x1, y1, x2, y2},
            4 * sizeof(int));

    return map->blockers_count - 1;
}END_OF_FUNCTION(add_blocker);

//gets the length and width of a matrix in a file
void get_dims(FILE* fp, int *set_x, int *set_y){
    *set_x = 0;
    *set_y = 1;
    char line[1000] = "";
    fgets(line, 1000, fp);
    int temp = 0, n, shift = 0;
    //loop through ln.1 to see how wide matrix is
    while(sscanf(&(line[0])+shift, "%d%n", &temp,&n) == 1){
        (*set_x)++;
        shift += n;
    }
    //loop through data to see length of matrix
    while(fgets(line, 1000, fp) != NULL){
        (*set_y)++;
    }
    rewind(fp);
}END_OF_FUNCTION(get_dims);

//loop through wall values and look for if the item is a wall or not
bool is_wall(int val, int* walls, int size_walls){
    int i = 0;
    for(; i < size_walls; ++i){
        if (val == walls[i]){
            return true;
        }
    }
    return false;
}END_OF_FUNCTION(is_wall);

bool check_collisions(Game* game, TMX_map* map, Sprite* sprite, int except){
    /*wider checks if the sprite is rotated 90 degrees, if yes we rotate the blocker
    within 45 and 135 degrees and 225 and 315 degrees is wider
    note these values are adjusted to be equivelent to 255 deg in a circle
    because allegro*/
    bool wider = (sprite->last_angle > 31.875 && sprite->last_angle < 92.625) ||
                 (sprite->last_angle > 159.375 && sprite->last_angle < 223.125);
    //get sprite rectangle locations
    float x1, y1, x2, y2;

    x1 = sprite->x;
    y1 = sprite->y;

    x2 = wider ? x1 + sprite->img_h : x1 + sprite->img_w;
    y2 = wider ? y1 + sprite->img_w : y1 + sprite->img_h;

    /*we use overlap checking to determine which side gets blocked

    COLLISION BLOCKS
    ================
    _____________
    |         __|____
    |         | |   |
    |_________|_|   |
              |     |
              |_____|
              xoverlap = 1 unit(s)
              yoverlap = 2 unit(s)

              since xoverlap is less
              x side is blocked
    RESULTANT BLOCKS
    ================
    _____________
    |           |_______
    |           ||     |
    |___________||     |
                 |     |
                 |_____|
    */
    float xoverlap, yoverlap;
    bool topCol, botCol, leftCol, rightCol;
    int i = 0;
    bool rval = false;
    for(; i < map->blockers_count; ++i){
        if (i == except){
            continue;
        }
        //set overlaps to essentially infinity (for our purposes)
        xoverlap = FLT_MAX;
        yoverlap = FLT_MAX;

        //draw red rectangles around blockers if debugging
        if (game->debug){
            rect(game->buffer, map->blockers[i][0] - game->cam_x,
                 map->blockers[i][1]-game->cam_y,
                 map->blockers[i][2]-game->cam_x,
                 map->blockers[i][3]-game->cam_y, makecol(0,0,255));
            textprintf_ex(game->buffer, font, map->blockers[i][0] - game->cam_x,
                           map->blockers[i][1]-game->cam_y,
                           makecol(255, 255, 255), -1, "%d", i);
        }

        //check for general collision
        if(x1 < map->blockers[i][2] &&
           x2 > map->blockers[i][0] &&
           y1 < map->blockers[i][3] &&
           y2 > map->blockers[i][1]){
            //check which sides are being collided with
            topCol = (y2 > map->blockers[i][1] && y2 < map->blockers[i][3]);
            botCol = (y1 < map->blockers[i][3] && y1 > map->blockers[i][1]);

            leftCol = (x2 > map->blockers[i][0] && x2 < map->blockers[i][2]);
            rightCol = (x1 < map->blockers[i][2] && x1 > map->blockers[i][0]);


            //xor top collision and bottom collision
            if (topCol ^ botCol){
                //get y overlap based on collision side
                if (topCol){
                    yoverlap = y2 - map->blockers[i][1];
                }else{
                    yoverlap = (y1 - map->blockers[i][3]) * -1;
                }

            }

            //same concept as above
            if(leftCol ^ rightCol){
                if (leftCol){
                    xoverlap = x2 - map->blockers[i][0];
                }else{
                    xoverlap = (x1 - map->blockers[i][2]) * -1;
                }
            }

            //check the lesser value and set sprite location to edge
            if(xoverlap > yoverlap){
                sprite->y = topCol ? map->blockers[i][1] -
                                     (sprite->img_h + 2):
                                     map->blockers[i][3] + 2;
            }else{
                sprite->x = leftCol ? map->blockers[i][0] -
                                      (sprite->img_w + 2) :
                                      map->blockers[i][2] + 2;
            }

            //recalculate path
            calc_velocity(sprite, sprite->ldist);
            rval = true;
        }
    }

    return rval;
}END_OF_FUNCTION(check_collisions);
