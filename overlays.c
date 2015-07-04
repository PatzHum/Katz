#include "ngine.h"
/*Overlays
Essentially overlays are timed custom layers
When creating an overlay, we get:

    * width
    * height
    * x location
    * y location
    * time
    * speed

Most of these are self explanitory
TIME is the number of ticks before the overlay is deleted
SPEED is the amount the overlay moves vertically while still active*/

//removes an overlay resizes overlays to save memory
void remove_overlay(Game* game, int index){
    int i = index;
    destroy_bitmap(game->overlays[i].img);
    for(; i < game->overlay_count; ++i){
        game->overlays[i] = game->overlays[i+1];
    }
    game->overlays = (Overlay*) realloc(game->overlays,
                                        --(game->overlay_count) *
                                        sizeof(Overlay));
}END_OF_FUNCTION(remove_overlay);

//adds a overlay to game->overlays
int add_overlay(Game* game, int w, int h, int x, int y, int time, int speed){
    int index = game->overlay_count;
    /*resize overlays*/
    game->overlays = (Overlay*) realloc(game->overlays,
                                        ++(game->overlay_count) *
                                        sizeof(*game->overlays));

    //initialize new overlay
    game->overlays[index].img = create_bitmap(w, h);
    clear_to_color(game->overlays[index].img, makecol(255, 0, 255));
    game->overlays[index].x = x + game->cam_x;
    game->overlays[index].y = y + game->cam_y;
    game->overlays[index].time = time;
    game->overlays[index].speed = speed;

    return index;
}END_OF_FUNCTION(add_overlay);

void blit_overlays(Game* game){
    //loop through overlays and blit to screen
    int i = game->overlay_count - 1;
    for(; i > -1; i--){
        masked_blit(game->overlays[i].img, game->buffer, 0, 0,
                    game->overlays[i].x - game->cam_x,
                    game->overlays[i].y - game->cam_y,
                    game->overlays[i].img->w,
                    game->overlays[i].img->h);
        game->overlays[i].y -= game->overlays[i].speed;
        if (--game->overlays[i].time <=0){
            remove_overlay(game, i);
        }
    }
}END_OF_FUNCTION(blit_overlays);
