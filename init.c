#include "ngine.h"

void game_init(Game* game){
    fprintf(fpLog, "Initializing game...\n");
    //defaults before being set
    game->running = true;
    game->screen_h = 1;
    game->screen_w = 1;
    game->overlay_count = 0;
    game->overlays = NULL;
    game->ticks = 0;
    game->delay = 16;
    game->cam_x = 0;
    game->cam_y = 0;
    game->debug = false;
    game->pan_speed = 8;
    game->car_switch_delay = 0;
    game->in_car = false;
    game->NPCs = malloc(0);
    game->npc_count = 0;
    game->debug_cd = 0;
    game->music = true;
    game->npc_killed = 0;
    game->difficulty = 1;

    game->max_ticks = TICK_MAX;
    game->ticks_left = TICK_MAX;

    strcpy(game->active_map, "level_1");

    if ((game->bgMusic = load_sample("res/sound/bg.wav")) == 0){
        fprintf(fpLog, "Music Failed\n");
    }

    play_sample(game->bgMusic, 255, 128, 1000, 1);

    //Read in settings from a file
    FILE* file_settings = fopen("res/settings", "r");
    if(game->debug && file_settings == NULL){
        fprintf(fpLog, "File not loaded SETTINGS\n");
    }
    char setting[128];

    //Loop through file
    while(fscanf(file_settings, "%s", setting) != EOF){
        if(!strcmp(setting, "SCREEN_SIZE")){
            fscanf(file_settings, "%d %d", &game->screen_w, &game->screen_h);
        }else if(!strcmp(setting, "START_MAP")){
            fscanf(file_settings, "%s", game->active_map);
        }else if(!strcmp(setting, "DEBUG")){
            fscanf(file_settings, "%d", &game->debug);
        }else if(!strcmp(setting, "PAN_SPEED")){
            fscanf(file_settings, "%d", &game->pan_speed);
        }else if(!strcmp(setting, "DELAY")){
            fscanf(file_settings, "%d", &game->delay);
        }else{
            fprintf(fpLog, "Failed to set setting <%s> from </res/settings>\n",
                   setting);
        }
    }

    //initialize screen & buffer
    set_color_depth(desktop_color_depth());
    set_gfx_mode(GFX_AUTODETECT_WINDOWED,
                (int) game->screen_w * STRETCH_FACTOR, (int) game->screen_h *
                  STRETCH_FACTOR, 0, 0);
    game->buffer = create_bitmap(game->screen_w, game->screen_h);
}END_OF_FUNCTION(game_init);

void allegro_init_install(){
    fprintf(fpLog, "Initializing Allegro Libraries\n");
    //Allegro initializations
    if(allegro_init() != 0){
        fprintf(fpLog, "Failed to initialize Allegro\n");
    }

    //keyboard
    if(install_keyboard() != 0){
        fprintf(fpLog, "Failed to initialize Allegro Keyboard\n");
    }

    //mouseq
    if(install_mouse() < 0){
        fprintf(fpLog, "Failed to initialize Allegro Mouse\n");
    }

    //music
    if(install_sound(DIGI_AUTODETECT, MIDI_AUTODETECT, "") != 0){
        fprintf(fpLog, "Failed to install sound\n");
    }

    set_window_title("Katz");
}END_OF_FUNCTION(allegro_init_install);


void player_init(Game* game, Sprite* player, TMX_map* map){
    //set default player variables, all arbitrary
    player->alive = true;
    player->hp = 100;
    player->hp_max = 100;
    fprintf(fpLog, "Initializing player...\n");
    player->x = 0;
    player->y = 0;
    player->tx = 0;
    player->ty = 0;
    player->dx = 0;
    player->dy = 0;

    player->img_w = 10;
    player->img_h = 11;

    player->speed = 1;

    player->ldist = 0;

    player->wlk_loop = 0;

    player->last_angle = 0;

    player->bullets = NULL;
    player->bullet_count = 0;
    player->bullet_cd = 0;

    player->img = load_bitmap("res/img/char/char.bmp", NULL);
    if(game->debug && player->img == NULL){
        fprintf(fpLog, "Image not loaded char.bmp\n");
    }
    //we set this to -1 because the player can only run into walls
    //and can never be a wall itself
    player->blocker_index = -1;

    player->npc = false;
}END_OF_FUNCTION(player_init);

void display_instructions(Game* game){
    //print various instructions for user
    printf("DEVELOPER OPTIONS:\n");
    printf("Press <F1> to toggle logging, CURRENTLY %s."
           "\nTo change this edit /res/settings\n",
           game->debug ? "ENABLED" : "DISABLED");
    printf("Press <F5> to reset the game.\n");

    print_instructions(game, "\"Kats\"\nBy : Patrick Hum", true);
    print_instructions(game, "Objective:\n"
                             " \n1. Turn all the humans \n"
                             "into kats before the time runs out\n"
                             "2. ???\n3. Profit", true);

    print_instructions(game, "Use <WASD> / <RIGHT CLICK> to move camera\n"
                             "Use <LEFT CLICK> to move around\n"
                             "Press <E> to enter your car\n"
                             "Press <SPACE> to shoot the gat "
                             "(turns people into cats)", true);
    print_instructions(game, "Press <ESC> or <Q> to exit\n"
                             "Press <M> to toggle music", true);

}END_OF_FUNCTION(display_instructions);
