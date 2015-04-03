#include <stdio.h>

#include "main.h"
#include "memory.h"
#include "sprite.h"
#include "utilities.h"


static int end_gameplay = 0;


typedef enum
{
    NO_COLOR = -1,
    COLOR_RED = 0,
    COLOR_ORA,
    COLOR_YEL,
    COLOR_GRE,
    COLOR_BLU,
    COLOR_PUR,
    TOTAL_COLORS
} COLORS;


typedef struct
{
    SPRITE sprite;

    /* A character cannot pass through a solid tile */
    int solid;

    /* The tile will be removed if hits is 0 */
    /* Indestructable if < 0 */
    int hits;

    /* Color is used to compare to stars */
    int color;
} TILE;


static TILE *board[ROWS][COLS];


typedef struct
{
    float x;
    float y;
    int w;
    int h;
} BODY;


typedef struct
{
    SPRITE sprite;
    int color;

    int hits;

    BODY body;

    int is_moving;
    int is_forward;
} STAR;


typedef struct
{
    SPRITE sprite;

    BODY body;

    /* Movement toggles */
    /* True means the hero is moving in that direction */
    int u;
    int d;
    int l;
    int r;

    int is_shooting;

    STAR *star;
} HERO;


static HERO hero;

#define MAX_STARS 32
static STAR *stars[MAX_STARS];


typedef struct
{
    HERO hero;
    STAR *stars[MAX_STARS];
    TILE *board[ROWS][COLS];
} SCENE;


int init_scene(SCENE *scene)
{
    return EXIT_FAILURE;
}


static int get_hero_speed()
{
    /* The hero can move four tiles in one second */
    return TILE_SIZE * 4;
}


static int get_star_speed()
{
    return TILE_SIZE * 10;
}


static float convert_pps_to_fps(int pps)
{
    return pps / (float)(GAME_TICKER);
}


IMAGE *get_star_image(int color, int frame)
{
    static char *star_image_names[6][2] = {
        {"star-red-1.png", "star-red-2.png"},
        {"star-orange-1.png", "star-orange-2.png"},
        {"star-yellow-1.png", "star-yellow-2.png"},
        {"star-green-1.png", "star-green-2.png"},
        {"star-blue-1.png", "star-blue-2.png"},
        {"star-purple-1.png", "star-purple-2.png"}
    };

    return IMG(star_image_names[color][frame]);
}


STAR *create_star(int color)
{
    STAR *star = alloc_memory("STAR", sizeof(STAR));

    init_sprite(&star->sprite, 1, 4);
    add_frame(&star->sprite, get_star_image(color, 0));
    add_frame(&star->sprite, get_star_image(color, 1));

    star->color = color;
    star->is_moving = 0;
    star->is_forward = 0;
    star->hits = 2;
    star->body.x = 0;
    star->body.y = 0;
    star->body.w = TILE_SIZE - 1;
    star->body.h = TILE_SIZE - 1;

    return star;
}


STAR *destroy_star(STAR *star)
{
    return free_memory("STAR", star);
}


int init_hero(HERO *hero, float x, float y)
{
    init_sprite(&hero->sprite, 1, 10);
    add_frame(&hero->sprite, IMG("makayla-01.png"));
    add_frame(&hero->sprite, IMG("makayla-02.png"));
    hero->sprite.x_offset = -10;
    hero->sprite.y_offset = -10;

    /* Set the starting position */
    hero->body.x = x;
    hero->body.y = y;

    hero->body.w = 10;
    hero->body.h = 10;

    hero->u = 0;
    hero->d = 0;
    hero->l = 0;
    hero->r = 0;

    hero->is_shooting = 0;

    hero->star = NULL;

    return EXIT_SUCCESS;
}


int init_stars()
{
    int i;

    for (i = 0; i < MAX_STARS; i++) {
        stars[i] = NULL;
    }

    return EXIT_SUCCESS;
}


int random_front_color()
{
    int r, c;
    int colors[TOTAL_COLORS];
    int num_colors = 0;
    int i;
    int exists;

    /**
     * TODO:
     * Take in to account that the hero might be about to destroy
     * the last block of this color!
     *
     * if (only one block of this block color is available to hit)
     *   and
     * if (it's the same color as the previous star color)
     *     remove the color from the list of available colors
     */ 
    r = 0;
    while (r < ROWS) {
        c = 0;
        while (c < COLS) {
            if (board[r][c] != NULL && board[r][c]->color >= 0) {

                exists = 0;

                for (i = 0; i < num_colors && !exists; i++) {
                    if (colors[i] == board[r][c]->color) {
                        exists = 1;
                    }
                }

                if (!exists) {
                    colors[num_colors] = board[r][c]->color;
                    num_colors++;
                }

                c = COLS;
            } else {

                c++;

            }
        }

        r++;

    }

    return colors[random_number(0, num_colors - 1)];
}


int set_hero_star(HERO *hero, STAR *star)
{
    if (hero->star != NULL) {
        hero->star = destroy_star(hero->star);
    }

    hero->star = star;

    return EXIT_SUCCESS;
}


int shoot_star(int color, float x, float y)
{
    int i;

    for (i = 0; i < MAX_STARS; i++) {
        if (stars[i] == NULL) {
            stars[i] = create_star(color);
            stars[i]->body.x = x;
            stars[i]->body.y = y;
            stars[i]->is_moving = 1;
            stars[i]->is_forward = 1;
            return EXIT_SUCCESS;
        }
    }

    return EXIT_FAILURE;
}


void control_hero(HERO *hero, ALLEGRO_EVENT *event)
{
    int key = 0;

    if (event->type == ALLEGRO_EVENT_KEY_DOWN) {
        key = event->keyboard.keycode;

        /* Hero key pressed */
        switch (key) {
            case ALLEGRO_KEY_UP:
                hero->u = 1;
                break;
            case ALLEGRO_KEY_DOWN:
                hero->d = 1;
                break;
            case ALLEGRO_KEY_LEFT:
                hero->l = 1;
                break;
            case ALLEGRO_KEY_RIGHT:
                hero->r = 1;
                break;
            case ALLEGRO_KEY_SPACE:
                hero->is_shooting = 1;
                break;
        }
    }

    if (event->type == ALLEGRO_EVENT_KEY_UP) {
        key = event->keyboard.keycode;

        /* Hero key released */
        switch (key) {
            case ALLEGRO_KEY_UP:
                hero->u = 0;
                break;
            case ALLEGRO_KEY_DOWN:
                hero->d = 0;
                break;
            case ALLEGRO_KEY_LEFT:
                hero->l = 0;
                break;
            case ALLEGRO_KEY_RIGHT:
                hero->r = 0;
                break;
        }
    }
}


IMAGE *get_block_image(int color)
{
    static char *block_image_names[6] = {
        "block-red.png",
        "block-orange.png",
        "block-yellow.png",
        "block-green.png",
        "block-blue.png",
        "block-purple.png"
    };

    return IMG(block_image_names[color]);
}


void setup_room(int num_cols, int num_colors)
{
    TILE *tile;
    int r, c;

    /* Board */
    for (r = 0; r < ROWS; r++) {
        for (c = 0; c < COLS; c++) {
            if (c == 0 || r == 0 || c == COLS - 1 || r == ROWS - 1) {
                tile = alloc_memory("TILE", sizeof(TILE));
                init_sprite(&tile->sprite, 0, 0);
                add_frame(&tile->sprite, IMG("bricks.png"));

                tile->solid = 1;
                tile->color = NO_COLOR;
                tile->hits = -1;

                board[r][c] = tile;
            } else {
                board[r][c] = NULL;
            }
        }
    }

    /* Add blocks */
    for (r = 1; r < ROWS - 1; r++) {
        for (c = 0; c < num_cols; c++) {
            tile = alloc_memory("TILE", sizeof(TILE));

            tile->solid = 1;
            tile->hits = 1;
            tile->color = random_number(0, num_colors - 1);

            init_sprite(&tile->sprite, 0, 0);
            add_frame(&tile->sprite, get_block_image(tile->color));

            board[r][COLS - 2 - c] = tile;
        }
    }
}


int new_game()
{
    init_hero(&hero, TILE_SIZE, TILE_SIZE);
    init_stars();

    /* Init the board */
    setup_room(4, 5);

    set_hero_star(&hero, create_star(random_front_color()));

    return EXIT_SUCCESS;
}


void control_gameplay(void *data, ALLEGRO_EVENT *event)
{
    int key = 0;

    /* General application control */
    if (event->type == ALLEGRO_EVENT_KEY_DOWN) {
        key = event->keyboard.keycode;

        if (key == ALLEGRO_KEY_ESCAPE) {
            /* ESC : Quit the game */
            end_gameplay = 1;
        } else if (key == ALLEGRO_KEY_S) {
            /* S : Toggle audio */
            toggle_audio();
        }
    }

    /* Hero control */
    control_hero(&hero, event);
}


TILE *is_board_collision(BODY *body)
{
    TILE *tile = NULL;
    int r1 = (int)(body->y / TILE_SIZE);
    int c1 = (int)(body->x / TILE_SIZE);
    int r2 = (int)((body->y + body->h) / TILE_SIZE);
    int c2 = (int)((body->x + body->w) / TILE_SIZE);
    
    tile = board[r1][c1];
    if (tile && tile->solid) {
        return tile;
    }
    
    tile = board[r1][c2];
    if (tile && tile->solid) {
        return tile;
    }
    
    tile = board[r2][c1];
    if (tile && tile->solid) {
        return tile;
    }
    
    tile = board[r2][c2];
    if (tile && tile->solid) {
        return tile;
    }
    
    return NULL;
}


void update_hero(HERO *hero)
{
    float old_x = hero->body.x;
    float old_y = hero->body.y;

    /*
     * TODO:
     * Allow for key-holds and temp movement taps.
     */

    /* Vertical movement */
    if (!(hero->u && hero->d)) {
        if (hero->u) {
            hero->body.y -= convert_pps_to_fps(get_hero_speed());
        } else if (hero->d) {
            hero->body.y += convert_pps_to_fps(get_hero_speed());
        }
    }

    /* Check for vertical collisions */
    if (is_board_collision(&hero->body) != NULL) {
        hero->body.y = old_y;
    }

    /* Horizontal movement */
    if (!(hero->l && hero->r)) {
        if (hero->l) {
            hero->body.x -= convert_pps_to_fps(get_hero_speed());
        } else if (hero->r) {
            hero->body.x += convert_pps_to_fps(get_hero_speed());
        }
    }

    /* Check for horizontal collisions */
    if (is_board_collision(&hero->body) != NULL) {
        hero->body.x = old_x;
    }

    if (hero->is_shooting) {
        shoot_star(hero->star->color, hero->star->body.x, hero->star->body.y);
        set_hero_star(hero, create_star(random_front_color()));
        hero->is_shooting = 0;
    }

    /* Tell the hero's star to follow the hero */
    if (hero->star != NULL) {
        hero->star->body.x = hero->body.x + 20;
        hero->star->body.y = (((int)hero->body.y  + 5) / TILE_SIZE) * TILE_SIZE;
    }

    /* Graphics */
    animate(&hero->sprite);
    animate(&hero->star->sprite);
}


int update_star(STAR *star)
{
    TILE *tile = NULL;
    float old_x = star->body.x;
    float old_y = star->body.y;

    if (star->is_moving) {
        if (star->is_forward) {
            star->body.x += convert_pps_to_fps(get_star_speed());
        } else {
            star->body.x -= convert_pps_to_fps(get_star_speed());
        }
    }

    tile = is_board_collision(&star->body);

    if (tile != NULL) {

        /* A tile has been hit! */

        star->body.x = old_x;
        star->body.y = old_y;

        if (star->color == tile->color) {
            /* Matching colors! */
            /* Remove the star and tile */
            tile->hits--;
            star->hits = 0;
            play_sound(SND("star_hit.wav"));
        } else {
            /* Just bounce */
            star->hits--;
            star->is_forward = star->is_forward ? 0 : 1;
        }
    }

    animate(&star->sprite);

    return EXIT_SUCCESS;
}


int update_stars()
{
    STAR *star;
    int i, j;

    for (i = 0; i < MAX_STARS; i++) {

        star = stars[i];

        if (star != NULL) {

            update_star(star);

            /* Remove stars that have no hits left */
            if (star->hits <= 0) {
                stars[i] = destroy_star(star);
                play_sound(SND("star_disolve.wav"));
            }
        }
    }

    /* Remove any empty spaces in the array of stars */
    /* Keep things on screen looking nice and in order */
    for (i = 0; i < MAX_STARS; i++) {
        if (stars[i] == NULL) {
            for (j = i; j < MAX_STARS - 1; j++) {
                stars[j] = stars[j + 1];
            }
            stars[j] = NULL;
        }
    }

    return EXIT_SUCCESS;
}


int update_board()
{
    int r, c;
    TILE *tile;

    for (r = 0; r < ROWS; r++) {
        for (c = 0; c < COLS; c++) {

            tile = board[r][c];

            if (tile && tile->hits == 0) {
                /* Remove the tile */
                board[r][c] = free_memory("TILE", tile);
            }

        }
    }

    return EXIT_SUCCESS;
}


int update_gameplay(void *data)
{
    /* Hero */
    update_hero(&hero);

    /* Stars */
    update_stars();

    /* Board */
    update_board();

    return !end_gameplay;
}


void draw_gameplay(void *data)
{
    int r, c;
    int i;

    /* Draw the background behind everything else */
    for (r = 0; r < ROWS; r++) {
        for (c = 0; c < COLS; c++) {
            al_draw_bitmap(IMG("background.png"), c * TILE_SIZE, r * TILE_SIZE, 0);
        }
    }
    
    /* Draw the game board */
    for (r = 0; r < ROWS; r++) {
        for (c = 0; c < COLS; c++) {
            draw_sprite(&board[r][c]->sprite, c * TILE_SIZE, r * TILE_SIZE);
        }
    }
    
    /* Draw stars */
    for (i = 0; i < MAX_STARS; i++) {
        if (stars[i] != NULL) {
            draw_sprite(&stars[i]->sprite, stars[i]->body.x, stars[i]->body.y);
        }
    }

    /* Draw the hero */
    draw_sprite(&hero.sprite, hero.body.x, hero.body.y);
    if (hero.star != NULL) {
        draw_sprite(&hero.star->sprite, hero.star->body.x, hero.star->body.y);
    }
}


void cleanup_hero(HERO *hero)
{
    if (hero != NULL && hero->star != NULL) {
        hero->star = destroy_star(hero->star);
    }
}


void cleanup_gameplay(void *data)
{
    int r, c;
    int i;

    /* Destroy tiles */
    for (r = 0; r < ROWS; r++) {
        for (c = 0; c < COLS; c++) {
            board[r][c] = free_memory("TILE", board[r][c]);
        }
    }
    
    /* Destroy stars */
    for (i = 0; i < MAX_STARS; i++) {
        stars[i] = destroy_star(stars[i]);
    }

    /* Destroy the hero */
    cleanup_hero(&hero);
}
