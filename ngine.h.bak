#ifndef NGINE_H
#define NGINE_H

//Include Allegro libraries
#include <allegro.h>

//Standard I/O includes
#include <stdio.h>
#include <stdlib.h>

//Additional includes
#include <math.h>
#include <strings.h>
#include <float.h>

//sizeof macro
#define arr_size(arr) (sizeof(arr) / sizeof(arr[0]))

//max/min macro
#define max(a, b) a > b ? a : b
#define min(a, b) a < b ? a : b

//define boolean type
typedef int bool;
#define true 1
#define false 0

#define PI 3.14159

#define tile_size 16

#define STRETCH_FACTOR 1.5

#define TICK_MAX 15000

//NOTE: These structures are thoroughly commented in external documentation
//see Documentation.txt

typedef struct{
    BITMAP* img;
    int x, y;
    int time;
    int speed;
}Overlay;

typedef struct {
    float x, y;
    float dx, dy;
    BITMAP* img;
    float angle;
}Bullet;

typedef struct {
    bool alive;

    int hp;
    int hp_max;

    float x, y;
    int tx, ty; //target location
    float dx, dy; //delta x,y
    int img_w, img_h; //image width/height

    int speed;  //movement speed
    float ldist; //last distance to target

    int wlk_loop; //walk loop counter

    BITMAP* img;

    float last_angle;   //last angle for consistancy

    int blocker_index;

    bool npc;
    int npc_update_cd;  //time till update
    int npc_wander_dist;

    Bullet* bullets;
    int bullet_count;
    int bullet_cd;

}Sprite;

typedef struct {
    bool running;   //game is running

    int cam_x, cam_y;   //camera location

    int screen_w, screen_h; //screen dimentions
    char active_map[128];   //active map

    BITMAP* buffer; //buffer

    int overlay_count;  //count of overlays
    Overlay* overlays;  //store overlays

    long ticks; //game ticks

    int delay;  //game delay between frames

    int pan_speed;  //camera pan speed

    int debug_cd;
    bool debug; //if debug, show extra data to console

    Sprite* active_sprite;  //pointer to a sprite

    bool in_car;

    int car_switch_delay;

    Sprite* NPCs;
    int npc_count;
    int npc_killed;

    SAMPLE* bgMusic;
    bool music;

    int max_ticks;
    int ticks_left;

    int difficulty;
}Game;

typedef struct {
    BITMAP* tilemap;    //tilemap image
    BITMAP* map;

    int ** tmx; //tile matrix
    int l,w;

    int** blockers;
    int blockers_count;

    int* blocker_values;
    int blocker_val_count;

    int bgCol;
}TMX_map;


//declare a global log file
FILE* fpLog;

//init.c
void game_init(Game* game);
void allegro_init_install();
void player_init(Game* game, Sprite* player, TMX_map* map);
void display_instructions(Game* game);

//sprites.c
void sprite_sequence(Game* game, Sprite* sprite, TMX_map* map, bool enabled);
void calc_velocity(Sprite* sprite, float dist);
double sprite_dist(Sprite* sprite1, Sprite* sprite2);

//utils.c
void exit_sequence(Game* game);
double get_angle(int x1, int y1, int x2, int y2);
void mouse_click(Game* game, Sprite* sprite);
double get_dist(int x1, int y1, int x2, int y2);
void set_mouse_bounds(int x1, int y1, int x2, int y2);
void print_instructions(Game* game, char* str, bool pause);
void pan_camera(Game* game);
void destroy_bitmaps(Game* game, Sprite* player, TMX_map* map, Sprite* car);
void blit_progress_bar(BITMAP* bmp, int val, int maxval, int x,
                       int y, int w, int h, int col, int outline_col);
void game_tick(Game* game, TMX_map* map);

//overlays.c
int add_overlay(Game* game, int w, int h, int x, int y, int time, int speed);
void remove_overlay(Game* game, int index);
void blit_overlays(Game* game);

//blockers.c
void blockers_generate(int* wall_v, TMX_map *map);
void get_dims(FILE* fp, int *set_x, int *set_y);
bool check_collisions(Game* game, TMX_map* map, Sprite* sprite, int except);
int add_blocker(TMX_map* map, int x1, int y1, int x2, int y2);
bool is_wall(int val, int* walls, int size_walls);

//maps.c
void load_map(Game* game, TMX_map* map);
void gen_map(TMX_map* map);

//hud.c
void show_hud(Game* game, Sprite* player);

//gats.c
void sprite_shoot(Game* game, Sprite* sprite);
void bullet_sequence(Game* game, TMX_map* map, Sprite* sprite);
void remove_bullet(Sprite* sprite, int index);
void bullet_collision(Game* game, TMX_map* map, Sprite* sprite, int index);

//cars.c
void car_init(Game* game, Sprite* car, TMX_map* map, int x, int y, int speed);
void enter_car (Game* game, Sprite* player, Sprite* car, TMX_map* map);

//npcs.c
void gen_npcs(Game* game, TMX_map* map);
void npc_sequence(Game* game, TMX_map* map);

#endif // NGINE_H
