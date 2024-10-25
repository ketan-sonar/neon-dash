#include <stdio.h>

#include "raylib.h"
#include "raymath.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 450
#define GROUND_Y 430

#define PLAYER_JUMP_SPEED 450*1.7

#define GRAVITY 980*2

#define MAX_OBSTACLES 100

typedef struct Player {
    Vector2 size;
    Vector2 pos;
    Vector2 vel;
} Player;

typedef struct Obstacle {
    Vector2 pos;
    Vector2 vel;
    size_t frame_counter;
    size_t frame_speed;
    size_t current_frame;
} Obstacle;

Obstacle obstacles[MAX_OBSTACLES] = { 0 };

size_t score = 0;
bool game_over = false;

int main(void)
{
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Geometry Dash");

    Texture2D scarfy = LoadTexture("./assets/scarfy.png");
    Rectangle frame_rec = (Rectangle){ .x = 0.0f, .y = 0.0f, .width = scarfy.width/6.0f, .height = (float)scarfy.height };

    Texture2D background = LoadTexture("./assets/cyberpunk_street_background.png");
    Texture2D midground = LoadTexture("./assets/cyberpunk_street_midground.png");
    Texture2D foreground = LoadTexture("./assets/cyberpunk_street_foreground.png");

    float scrollingBack = 0.0f;
    float scrollingMid = 0.0f;
    float scrollingFore = 0.0f;

    Texture2D explosions[7];
    for (size_t i = 0; i < 7; ++i) {
        char buf[35];
        snprintf(buf, 35, "./assets/explosion/explosion_%c.png", '1'+(int)i);
        explosions[i] = LoadTexture(buf);
    }

    size_t explosion_frame_counter = 0;
    size_t explosion_current_frame = 0;
    size_t explosion_frame_speed = 8;

    Texture2D fireballs[5];
    for (size_t i = 0; i < 5; ++i) {
        char buf[33];
        snprintf(buf, 33, "./assets/fireball/fireball_%c.png", '1'+(int)i);
        fireballs[i] = LoadTexture(buf);
    }

    size_t spacing = 0;
    for (size_t i = 0; i < MAX_OBSTACLES; ++i) {
        obstacles[i].pos.x = spacing;
        obstacles[i].pos.y = GROUND_Y - fireballs[0].height * (GetRandomValue(0, 100) > 30 ? 0.5f : 1.5f);
        spacing += (size_t)fireballs[0].width + GetRandomValue(600, 800);

        obstacles[i].vel = (Vector2){ .x = 0, .y = 0 };

        obstacles[i].frame_counter = 0;
        obstacles[i].current_frame = 0;
        obstacles[i].frame_speed = 8;
    }

    Player player = {
        .size = { .x = scarfy.width, .y = scarfy.height },
        .pos  = { .x = WINDOW_WIDTH/2.0f, .y = WINDOW_HEIGHT/2.0f },
        .vel  = { .x = 400, .y = 0 },
    };

    Camera2D camera = { 0 };
    camera.target = player.pos;
    camera.offset = (Vector2){ .x = WINDOW_WIDTH/2.0f, WINDOW_HEIGHT/2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    size_t sprite_frame_counter = 0;
    size_t sprite_frame_speed = 8;
    size_t sprite_current_frame = 0;

    SetTargetFPS(60);
    while (!WindowShouldClose()) {
        if (game_over) {
            ++explosion_frame_counter;
            if (explosion_frame_counter >= (60/explosion_frame_speed)) {
                explosion_frame_counter = 0;
                if (explosion_current_frame > 5) {
                    goto drawing;
                }
            }
            goto drawing;
        }

        ++sprite_frame_counter;
        if (sprite_frame_counter >= (60/sprite_frame_speed)) {
            sprite_frame_counter = 0;
            if (++sprite_current_frame > 5) sprite_current_frame = 0;
            frame_rec.x = sprite_current_frame*scarfy.width/6.0f;
        }

        for (size_t i = 0; i < MAX_OBSTACLES; ++i) {
            obstacles[i].frame_counter++;
            if (obstacles[i].frame_counter >= (60/obstacles[i].frame_speed)) {
                obstacles[i].frame_counter = 0;
                obstacles[i].current_frame++;
                if (obstacles[i].current_frame > 4) obstacles[i].current_frame = 0;
            }

            obstacles[i].pos = Vector2Add(obstacles[i].pos, obstacles[i].vel);

            float playerLeft = player.pos.x - frame_rec.width/2.0f;
            float playerRight = playerLeft + frame_rec.width;
            float playerTop = player.pos.y;
            float playerBottom = playerTop + frame_rec.height;

            float obstacleLeft = obstacles[i].pos.x;
            float obstacleRight = obstacleLeft + fireballs[0].width * 0.3f;
            float obstacleTop = obstacles[i].pos.y;
            float obstacleBottom = obstacleTop + fireballs[0].height * 0.3f;

            if (playerRight > obstacleLeft &&
                playerLeft < obstacleRight &&
                playerBottom > obstacleTop &&
                playerTop < obstacleBottom) {
                game_over = true;
                break;
            }

            if (playerRight >= obstacleLeft) {
                score += 1;
            }
        }

        scrollingBack -= 0.1f*4;
        scrollingMid  -= 0.5f*4;
        scrollingFore -= 1.0f*4;

        if (scrollingBack <= -background.width*2) scrollingBack = 0;
        if (scrollingMid <= -midground.width*2) scrollingMid = 0;
        if (scrollingFore <= -foreground.width*2) scrollingFore = 0;

        float dt = GetFrameTime();

        camera.target.x = player.pos.x + 150;

        if (GetKeyPressed() == KEY_SPACE) {
            if (player.pos.y == GROUND_Y-player.size.y) {
                player.vel.y -= PLAYER_JUMP_SPEED;
            }
        }

        player.vel.y += GRAVITY * dt;
        player.pos = Vector2Add(player.pos, Vector2Scale(player.vel, dt));

        if (player.pos.y > GROUND_Y-player.size.y) {
            player.pos.y = GROUND_Y-player.size.y;
            player.vel.y = 0;
        }

drawing:
        BeginDrawing();

            ClearBackground(ColorTint(GetColor(0x052c46ff), GRAY));

            DrawTextureEx(background,
                          (Vector2){ .x = scrollingBack, .y = 20 }, 0.0f, 2.0f, GRAY);
            DrawTextureEx(background,
                          (Vector2){ .x = background.width*2 + scrollingBack, .y = 20 },
                          0.0f, 2.0f, GRAY);

            DrawTextureEx(midground,
                          (Vector2){ .x = scrollingMid, .y = 20 }, 0.0f, 2.0f, GRAY);
            DrawTextureEx(midground,
                          (Vector2){ .x = midground.width*2 + scrollingMid, .y = 20 },
                          0.0f, 2.0f, GRAY);

            DrawTextureEx(foreground,
                          (Vector2){ .x = scrollingFore, .y = 70 }, 0.0f, 2.0f, GRAY);
            DrawTextureEx(foreground,
                          (Vector2){ .x = foreground.width*2 + scrollingFore, .y = 70 },
                          0.0f, 2.0f, GRAY);

            BeginMode2D(camera);


                DrawTextureRec(scarfy, frame_rec,
                               (Vector2){ .x = player.pos.x - frame_rec.width/2.0f, .y = player.pos.y },
                               WHITE);

                for (size_t i = 0; i < MAX_OBSTACLES; ++i) {
                    DrawTextureEx(fireballs[obstacles[i].current_frame], obstacles[i].pos, 0, 0.3f, WHITE);
                }

                if (game_over) {
                    DrawTextureEx(explosions[explosion_current_frame],
                                  (Vector2){ .x = player.pos.x - explosions[explosion_current_frame].width*0.8f/2.0f, .y = GROUND_Y - explosions[explosion_current_frame].height*0.8f }, 0, 0.8f, WHITE);
                }
            
            EndMode2D();

            char score_txt[32] = { 0 };
            sprintf(score_txt, "Score: %ld", score);
            score_txt[31] = 0;
            DrawText(score_txt, 50, 50, 20, YELLOW);

        EndDrawing();
    }

    for (size_t i = 0; i < 5; ++i) {
        UnloadTexture(fireballs[i]);
    }

    for (size_t i = 0; i < 7; ++i) {
        UnloadTexture(explosions[i]);
    }

    UnloadTexture(background);
    UnloadTexture(midground);
    UnloadTexture(foreground);
    UnloadTexture(scarfy);

    CloseWindow();

    return 0;
}
