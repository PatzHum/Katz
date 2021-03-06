#include "ngine.h"

/*Gats shoot out cats*/
void sprite_shoot(Game* game, Sprite* sprite){
    float dist;

    sprite->bullets = (Bullet*) realloc(sprite->bullets,
                                        ++sprite->bullet_count *
                                        sizeof(Bullet));
    int index = sprite->bullet_count - 1;
    int tx, ty;
    tx = mouse_x + game->cam_x + text_length(font, "x") / 2;
    ty = mouse_y + game->cam_y + text_height(font) / 2;

    sprite->bullets[index].x = sprite->x;
    sprite->bullets[index].y = sprite->y;

    sprite->bullets[index].angle = get_angle(sprite->x, sprite->y, tx,ty);

    sprite->bullets[index].img = create_bitmap(2, 5);
    clear_to_color(sprite->bullets[index].img, makecol(0, 255, 255));

    dist = get_dist(sprite->bullets[index].x, sprite->bullets[index].y, tx,ty);

    sprite->bullets[index].dx = 10 * ((tx) - sprite->bullets[index].x) / dist;
    sprite->bullets[index].dy = 10 * ((ty) - sprite->bullets[index].y) / dist;

}END_OF_FUNCTION(player_shoot);

void bullet_sequence(Game* game, TMX_map* map, Sprite* sprite){
    int i = sprite->bullet_count - 1;
    for(; i > -1; --i){
        pivot_sprite_v_flip(game->buffer, sprite->bullets[i].img,
                            sprite->bullets[i].x - game->cam_x,
                            sprite->bullets[i].y - game->cam_y,
                            sprite->bullets[i].img->w / 2,
                            sprite->bullets[i].img->h / 2,
                                itofix(sprite->bullets[i].angle));
        sprite->bullets[i].x += sprite->bullets[i].dx;
        sprite->bullets[i].y += sprite->bullets[i].dy;

        if (sprite->bullets[i].x < 0 ||
            sprite->bullets[i].x > map->w * tile_size ||
            sprite->bullets[i].y < 0 ||
            sprite->bullets[i].y > map->w * tile_size){
            remove_bullet(sprite, i);
        }else{
            bullet_collision(game, map, sprite, i);
        }
    }
}END_OF_FUNCTION(bullet_sequence);

void remove_bullet(Sprite* sprite, int index){
    int i = index;
    destroy_bitmap(sprite->bullets[i].img);
    for (; i < sprite->bullet_count; ++i){
        sprite->bullets[i] = sprite->bullets[i+1];
    }
    sprite->bullets = (Bullet*) realloc(sprite->bullets,
                                        --sprite->bullet_count *
                                         sizeof(Bullet));
}END_OF_FUNCTION(remove_bullet);

void bullet_collision(Game* game, TMX_map* map, Sprite* sprite, int index){
    int x1, y1, x2, y2;
    x1 = sprite->bullets[index].x;
    y1 = sprite->bullets[index].y;
    x2 = sprite->bullets[index].x + sprite->bullets[index].img->w;
    y2 = sprite->bullets[index].y + sprite->bullets[index].img->h;

    int i = 0, j;
    for(; i < map->blockers_count; ++i){
        if(x1 < map->blockers[i][2] &&
           x2 > map->blockers[i][0] &&
           y1 < map->blockers[i][3] &&
           y2 > map->blockers[i][1]){
            for(j = 0; j < game->npc_count; ++j){
                if (game->NPCs[j].blocker_index == i && game->NPCs[j].alive){
                    game->NPCs[j].hp -= 5;
                    if (game->NPCs[j].hp <= 0){
                        destroy_bitmap(game->NPCs[j].img);
                        game->NPCs[j].img = load_bitmap(
                                            "res/img/npc/npc_dead.bmp", NULL);
                        game->NPCs[j].alive = false;
                        memcpy(map->blockers[i], (int[4]){-1, -1, -1, -1},
                                4 * sizeof(int));
                        game->npc_killed++;

                        if (game->npc_killed == game->npc_count){
                            free(game->NPCs);
                            game->NPCs = NULL;
                            game->npc_count = 0;
                            game->npc_killed = 0;
                            print_instructions(
                                game, "Level Passed. Can you keep it up?"
                                "\n-Remaining time added as bonus", true);

                            game->max_ticks = TICK_MAX + game->ticks_left;
                            game->ticks_left = game->max_ticks;

                            game->difficulty++;
                            gen_npcs(game, map);
                        }
                        break;
                    }
                }
            }
            remove_bullet(sprite, index);
            return;
        }
    }
}END_OF_FUNCTION(bullet_collision);
