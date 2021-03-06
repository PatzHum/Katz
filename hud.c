#include "ngine.h"

void show_hud(Game* game, Sprite* player){
    //shows a progress bar with the time coundown

    //the color difference, so we fade to red from green
    int diff = ((float)game->ticks_left / (float)game->max_ticks) * 255;

    //y offset for blitting difference
    int v_offset = 0;

    //add a progress bar for time
    blit_progress_bar(game->buffer, game->ticks_left,
                      game->max_ticks,
                      game->screen_w - 100, 0, 100, 10,
                      makecol(255 - diff, diff, 0), makecol(0, 0, 0));
    v_offset += 10 + 5;

    //Add a progress indicator for how many NPCs left
    textprintf_right_ex(game->buffer, font, game->screen_w - 5, v_offset,
                      makecol(255, 255, 255), -1,
                      "%d / %d", game->npc_killed, game->npc_count);
    v_offset += text_height(font) + 5;


}END_OF_FUNCTION(show_hud);
