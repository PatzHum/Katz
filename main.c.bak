#include "ngine.h"
//TODO
//--COMMENTS
int main(){

    //setup log file
    fpLog = fopen("log.txt", "w");
    if(fpLog == NULL){
        printf("Failed to open log file\n");
    }
    //seed random
    srand(time(NULL));

    //set up frame limiting
    clock_t s_cycle, e_cycle;
    s_cycle = clock();  //get amount of time to init game

    fprintf(fpLog, "Initializing...\n");    //log
    allegro_init_install(); //allegro inits

    Game game;
    game_init(&game);   //initialize game struct

    TMX_map map;
    load_map(&game, &map);  //load first map

    Sprite player, car;
    player_init(&game, &player, &map);   //initialize player struct
    car_init(&game, &car, &map, 32, 32, 5); //initialize car struct

    gen_npcs(&game, &map);  //generate and initialize npcs

    e_cycle = clock();
    //get length of initialization
    fprintf(fpLog, "Initialization done in %f seconds.\n",
            (double)(e_cycle - s_cycle) / 1000);

    //show instructions on how to play
    display_instructions(&game);

    //set active sprite
    game.active_sprite = &player;

    fprintf(fpLog, "Beginning main loop.\n");
    while(game.running){
        //to limit frames we have a set refresh rate
        //to maintain this refresh rate we subtract the
        //time elapsed over the entire cycle
        //to do this we get the time at the start of the cycle
        //and the end to get the difference
        s_cycle = clock();
        //exit sequence
        exit_sequence(&game);

        //check if we enter the car
        enter_car(&game, &player, &car, &map);

        //toggle debugging
        if(key[KEY_F1] && !game.debug_cd){
            game.debug = !game.debug;
            printf(game.debug ? "DEBUGGING ENABLED\n" : "DEBUGGING DISABLED\n");
            game.debug_cd = 10;
        }else{
            game.debug_cd -= game.debug_cd ? 1 : 0;
        }
        //resets game
        if(key[KEY_F5]){
            game_init(&game);
            load_map(&game, &map);
            player_init(&game, &player, &map);   //initialize player struct
            car_init(&game, &car, &map, 32, 32, 5);
            gen_npcs(&game, &map);
        }

        //toggle music
        if(key[KEY_M]){
            game.music ? stop_sample(game.bgMusic) :
                         play_sample(game.bgMusic, 255, 128, 1000, 1);
        }

        //clear buffer
        clear_to_color(game.buffer, makecol(0, 0, 0));

        //show map
        masked_blit(map.map, game.buffer, game.cam_x, game.cam_y,
                    0, 0, game.screen_w, game.screen_h);

        //pan camera
        pan_camera(&game);

        //do mouse click
        mouse_click(&game, game.active_sprite);
        npc_sequence(&game, &map);

        //display the player if we're not in the car
        if (!game.in_car){
            sprite_sequence(&game, &player, &map, !game.in_car);
            if (key[KEY_SPACE] && !player.bullet_cd){
                sprite_shoot(&game, &player);
                player.bullet_cd = 10;
            }else{
                player.bullet_cd -= player.bullet_cd ? 1 : 0;
            }
        }else{
            free(player.bullets);
            player.bullets = NULL;
            player.bullet_count = 0;
        }

        //show car
        sprite_sequence(&game, &car, &map, game.in_car);

        //display all overlays
        blit_overlays(&game);

        //show hud
        show_hud(&game, &player);

        //display buffer to screen
        stretch_blit(game.buffer, screen, 0, 0, game.buffer->w,
                     game.buffer->h, 0, 0, game.screen_w * STRETCH_FACTOR,
                     game.screen_h * STRETCH_FACTOR);

        //countdown timer
        game_tick(&game, &map);

        e_cycle = clock();

        /*maintain consistent framerate by calculating delay of
        processing, using max because we don't want negative rest*/
        printf("\rIteration took %f%20s", (double)
                (e_cycle - s_cycle), " ");
        rest(max(game.delay - (double) (e_cycle - s_cycle), 0));
    }

    fprintf(fpLog, "Running exit scripts...\n");
    //destroy all bitmaps
    destroy_bitmaps(&game, &player, &map, &car);
    fprintf(fpLog, "Exiting...\n");
    return 0;
}END_OF_MAIN();
