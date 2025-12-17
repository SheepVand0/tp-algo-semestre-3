/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "settings.h"
#include "game/game_config.h"
#include "game/core/game_core.h"

typedef struct Scene Scene;

#define GAME_GRID_SIZE g_gameConfig.Settings->GridSize
#define MAX_GAME_GRID_SIZE 40

#define SCREEN_WIDTH g_gameConfig.IsFullscreen ? FHD_WIDTH : HD_WIDTH
#define SCREEN_HEIGHT g_gameConfig.IsFullscreen ? FHD_HEIGHT : HD_HEIGHT

#define FHD_MUL_TWO (g_gameConfig.IsFullscreen ? 2 : 1)

#define GAME_GRAPHICS_RENDER(gameGraphics, rabbits, rabbcount) gameGraphics->RabbitsToRender = rabbits; \
gameGraphics->RabbitCount = rabbcount; \
gameGraphics->Selected = g_gameConfig.Selected; \
GameGraphics_render(gameGraphics); \


/// @brief Structure représentant la scène du menu principal du jeu.
typedef struct GameGraphics
{
    Scene* m_scene;
    AABB m_gridAABB;
    Vec2 m_padding;
    Vec2 m_spacing;

    AABB m_cells[MAX_GAME_GRID_SIZE][MAX_GAME_GRID_SIZE];

    int m_selectedRowIndex;
    int m_selectedColIndex;

    //SpriteGroup* m_spriteRabbit;

    Rabbit* RabbitsToRender;
    int RabbitCount;
    /*Obstacle** ObstaclesToRender;
    int ObstacleCount;*/

    Rabbit* Selected;

    SpriteGroup* HoverSprite;
    SpriteGroup* HoverSpriteFox;
    SpriteGroup* HoleSprite;
    SpriteGroup* MastermindSprite;
    SpriteGroup* OupiGoupiSprite;
    SpriteGroup* MinusFiveSprite;
    SpriteGroup* ExplosionSprite;

    SAudio* CandyBoomAudio;
    SAudio* HomeRunAudio;
    SAudio* EarthquakeAudio;
    SAudio* UndertaleBoomAudio;
    SAudio* StoneSlideAudio;
    SAudio* PunchesAudio[5];
    SAudio* ExplosionAudio;

    Vec2 LastWindowPosition;

    bool m_enabled;
    bool LeftPressed;
} GameGraphics;

GameGraphics* GameGraphics_create(Scene* scene);
void GameGraphics_destroy(GameGraphics* self);
void GameGraphics_update(GameGraphics* self);
void GameGraphics_render(GameGraphics* self);

INLINE void GameGraphics_setEnabled(GameGraphics* self, bool enabled)
{
    assert(self && "self must not be NULL");
    self->m_enabled = enabled;
}

float ModFloat(float v, float x);
