#include "ngine.h"

//sprite sequence calculate distance, move distance etc.
void sprite_sequence(Game* game, Sprite* sprite, TMX_map* map, bool enabled){
    float angle;
    int i;
    //get distance to destination location
    double dist_to_tp = get_dist(sprite->x, sprite->y, sprite->tx, sprite->ty);

    //if the distance becomes greater (we overshot the location) we stop
    if(dist_to_tp > (sprite->ldist + 1)){
        dist_to_tp = 0;
    }

    //set last distance
    sprite->ldist = dist_to_tp;

    if (enabled){
        if(!sprite->npc){
            //get angle to target
            angle = get_angle(sprite->x, sprite->y,
                                    (int)dist_to_tp ? sprite->tx :
                                    mouse_x + game->cam_x,
                                    (int)dist_to_tp ? sprite->ty :
                                    mouse_y + game->cam_y);
            sprite->last_angle = angle;
        }else{
            angle = get_angle(sprite->x, sprite->y, sprite->tx, sprite->ty);
        }
        //if there's distance left, we move the sprite
        if (dist_to_tp){
            for(i = 0; i < sprite->speed; ++i){
                sprite->x += sprite->dx / (float) sprite->speed;
                sprite->y += sprite->dy / (float) sprite->speed;
                //check collisions
                if (check_collisions(game, map, sprite, sprite->blocker_index)){
                    break;
                }
            }
        }

        //every 5 ticks add to walk loop
        if (game->ticks % 10 == 0){
            ++sprite->wlk_loop;
        }



        //recalculate collision rectangle for sprite
        //see explaination in BLOCKERS.C under function check collisions
        bool wider = (angle > 31.875 && angle < 92.625) ||
                    (angle > 159.375 && angle < 223.125);

        int x1, y1, x2, y2;

        x1 = sprite->x - (wider ? abs(sprite->img_w - sprite->img_h) / 2 : 0);
        y1 = sprite->y + (wider ? abs(sprite->img_w - sprite->img_h) / 2 : 0);

        x2 = wider ? x1 + sprite->img_h : x1 + sprite->img_w;
        y2 = wider ? y1 + sprite->img_w : y1 + sprite->img_h;

        if (sprite->blocker_index > -1)
            memcpy(map->blockers[sprite->blocker_index],
            (int[4]){x1, y1, x2, y2}, 4 * sizeof(int));

        int olay_index = add_overlay(game,
                                     sprite->img_w, 4,
                                     sprite->x - game->cam_x,
                                     sprite->y - game->cam_y - 4, 1, 0);
        blit_progress_bar(game->overlays[olay_index].img,
                          sprite->hp,
                          sprite->hp_max, 0, 0,
                          game->overlays[olay_index].img->w,
                          game->overlays[olay_index].img->h,
                          makecol(0, 255, 0), makecol(255, 0, 255));
    }else{
        angle = sprite->last_angle;
    }

    //create sprite image
    BITMAP* img = create_bitmap(sprite->img_w, sprite->img_h);
    //if we're walking blit appropriate image to img, otherwise default to 0
    blit(sprite->img, img,
         dist_to_tp && sprite->img->w / (int) sprite->img_w != 1 ?
         (sprite->wlk_loop % ((int) sprite->img->w /
         (int) sprite->img_w - 1)+ 1) * sprite->img_w : 0,
         0, 0, 0, sprite->img_w, sprite->img_h);


    //rotate image to mouse location and blit
    pivot_sprite(game->buffer, img,
                        sprite->x - game->cam_x + img->w / 2,
                        sprite->y - game->cam_y + img->h / 2,
                        img->w / 2, img->h / 2, itofix(sprite->last_angle));


    destroy_bitmap(img);

    bullet_sequence(game, map, sprite);

}END_OF_FUNCTION(sprite_sequence);

void calc_velocity(Sprite* sprite, float dist){
    sprite->dx = (sprite->speed * (sprite->tx - sprite->x))/(dist);
    sprite->dy = (sprite->speed * (sprite->ty - sprite->y))/(dist);
}END_OF_FUNCTION(calc_velocity);

double sprite_dist(Sprite* sprite1, Sprite* sprite2){
    bool wider = (sprite1->last_angle > 31.875 && sprite1->last_angle < 92.625) ||
                 (sprite1->last_angle > 159.375 && sprite1->last_angle < 223.125);

    //get sprite1 rectangle locations
    float x1, y1, x2, y2;

    x1 = sprite1->x + (wider ? sprite1->img_h : sprite1->img_w) / 2;
    y1 = sprite1->y + (wider ? sprite1->img_w : sprite1->img_h) / 2;

    wider = (sprite2->last_angle > 31.875 && sprite2->last_angle < 92.625) ||
                 (sprite2->last_angle > 159.375 && sprite2->last_angle < 223.125);

    x2 = sprite2->x + (wider ? sprite2->img_h : sprite2->img_w) / 2;
    y2 = sprite2->y + (wider ? sprite2->img_w : sprite2->img_h) / 2;

    return get_dist(x1, y1, x2, y2);

}END_OF_FUNCTION(sprite_dist);
