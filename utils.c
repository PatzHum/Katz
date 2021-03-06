#include "ngine.h"

void exit_sequence(Game* game){
    //check exit keypress
    if(key[KEY_ESC] || key[KEY_Q]){
        fprintf(fpLog, "Game paused.\n");
        stop_sample(game->bgMusic);
        //ensure exit
        textprintf_ex(screen, font, 5, 5,
                      makecol(255, 255, 255), -1,
                      "Press Q again to quit.");
        clear_keybuf();
        if(readkey() >> 8 == KEY_Q){
            game->running = false;
        }else{
            play_sample(game->bgMusic, 255, 128, 1000, 1);
            fprintf(fpLog, "Game resumed.\n");
        }
    }
}END_OF_FUNCTION(exit_sequence);

//gets the angle between two points
double get_angle(int x1, int y1, int x2, int y2){
    //get x and y differences
    float dx = x2 - x1;
    float dy = y2 - y1;

    //return calculations
    if (y2 >= y1){
        return (-atan(dx / dy) * 180 / PI) / 360 * 255;
    }else{
        return (180 - atan(dx / dy) * 180 / PI) / 360 * 255;
    }
}END_OF_FUNCTION(get_angle);

//distance formula
double get_dist(int x1, int y1, int x2, int y2){
    return sqrt((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1));
}END_OF_FUNCTION(get_dist);

void mouse_click(Game* game, Sprite* sprite){
    //display x as cursor
    textprintf_ex(game->buffer, font, mouse_x, mouse_y,
                  makecol(255, 255, 255), -1, "x");

    //if left click
    if(mouse_b & 1){
        //add overlay to screen of clicked location, 25 cycles
        int index = add_overlay(game, text_length(font, "x"),
                                text_height(font),
                                mouse_x, mouse_y, 25, 0);
        textprintf_ex(game->overlays[index].img, font, 0, 0,
                      makecol(75, 255, 0), -1, "x");

        //set destination
        sprite->tx = mouse_x + game->cam_x - text_length(font, "x") / 2;
        sprite->ty = mouse_y + game->cam_y - text_height(font) / 2;

        //get distance to point
        double dist_to_tp = get_dist(sprite->x,
                                     sprite->y,
                                     sprite->tx,
                                     sprite->ty);
        //set last distance
        sprite->ldist = dist_to_tp;

        //set delta x, y (speed x, y)
        calc_velocity(sprite, dist_to_tp);
    }

    //if right click center camera
    if(mouse_b & 2){
        game->cam_x += (mouse_x - game->screen_w / 2) / 20;
        game->cam_y += (mouse_y - game->screen_h / 2) / 20;
    }
}END_OF_FUNCTION(mouse_click);

//keeps mouse in set rectangle - somewhat buggy
void set_mouse_bounds(int x1, int y1, int x2, int y2){
    if(mouse_x > x2){
        position_mouse(x2 - 1, mouse_y);
    }else if(mouse_x < x1){
        position_mouse(x1 + 1, mouse_y);
    }

    if(mouse_y < y1) {
        position_mouse(mouse_x, y1 + 1);
    }else if(mouse_y > y2) {
        position_mouse(mouse_x, y2 - 1);
    }

}END_OF_FUNCTION(set_mouse_bounds);

/*prints instructions to screen
option of data output, and pause screen*/
void print_instructions(Game* game, char* str, bool pause){
    clear(game->buffer);
    char print_str[256];
    strcpy(print_str, str);
    char *temp;
    int y_offset = 0;
    temp = strtok(print_str, "\n");
    while(temp != NULL){
        textprintf_centre_ex(game->buffer, font,
                             game->screen_w/2,
                             game->screen_h/2 + y_offset,
                             makecol(0, 255, 0),
                             -1, "%s", temp);
        y_offset += text_height(font) + 5;
        temp = strtok(NULL, "\n");
    }

    textprintf_centre_ex(game->buffer, font,
                         game->screen_w/2,
                         game->screen_h/2 + text_height(font) + 50 + y_offset,
                         makecol(255, 255, 255), -1,
                         "Press ENTER to continue...");
    acquire_screen();
    stretch_blit(game->buffer, screen, 0, 0, game->buffer->w,
                 game->buffer->h, 0, 0, SCREEN_W, SCREEN_H);
    release_screen();

    if (pause){
        while((readkey() >> 8 )!= KEY_ENTER){
        }
    }

}END_OF_FUNCTION(print_instructions);

//pan camera around scren based off keypress
void pan_camera(Game* game){
    if(key[KEY_DOWN] || key[KEY_S]){
        game->cam_y += game->pan_speed;
    }
    if(key[KEY_UP] || key[KEY_W]){
        game->cam_y -= game->pan_speed;
    }
    if(key[KEY_RIGHT] || key[KEY_D]){
        game->cam_x += game->pan_speed;
    }
    if(key[KEY_LEFT] || key[KEY_A]){
        game->cam_x -= game->pan_speed;
    }
}END_OF_FUNCTION(pan_camera);

//destroy all bitmaps
void destroy_bitmaps(Game* game, Sprite* player, TMX_map* map, Sprite* car){
    fprintf(fpLog, "Destroying Bitmaps...\n");
    destroy_bitmap(game->buffer);
    int i = 0;
    for(; i < game->overlay_count; ++i){
        destroy_bitmap(game->overlays[i].img);
    }
    destroy_bitmap(player->img);
    destroy_bitmap(map->map);
    destroy_bitmap(map->tilemap);
    destroy_bitmap(car->img);

}END_OF_FUNCTION(destroy_bitmaps);

//Add a progress bar, essentially two rectangles
void blit_progress_bar(BITMAP* bmp, int val, int maxval,
                       int x, int y, int w, int h, int col, int outline_col){
    BITMAP* p_bar = create_bitmap(w, h);
    clear_to_color(p_bar, makecol(255, 0, 255));
    rectfill(p_bar, 0, 0,
             (int) (((float) val/ (float) maxval) * (float)w), h, col);
    rect(p_bar, 0, 0, w - 1, h - 1, outline_col);
    masked_blit(p_bar, bmp, 0, 0, x, y, w, h);
    destroy_bitmap(p_bar);
}END_OF_FUNCTION(progress_bar);

//Game tick function, adds ticks and subtracts ticks from timer
void game_tick(Game* game, TMX_map* map){
    game->ticks++;
    game->ticks_left -= game->ticks_left ? 1 : 0;
    if (game->ticks_left == 0){
        free(game->NPCs);
        game->NPCs = NULL;
        game->npc_count = 0;
        game->npc_killed = 0;
        game->max_ticks = TICK_MAX;
        game->ticks_left = game->max_ticks;
        print_instructions(game, "LEVEL FAILED - "
                           "You ran out of time\nPress <Enter> to retry",
                           true);
        gen_npcs(game, map);
    }
}END_OF_FUNCTION(game_tick);

