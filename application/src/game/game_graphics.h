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

#define GAME_GRID_SIZE g_gameConfig.settings->GridSize
#define MAX_GAME_GRID_SIZE 40

#define SCREEN_WIDTH g_gameConfig.isFullscreen ? FHD_WIDTH : HD_WIDTH
#define SCREEN_HEIGHT g_gameConfig.isFullscreen ? FHD_HEIGHT : HD_HEIGHT

#define FHD_MUL_TWO (g_gameConfig.isFullscreen ? 2 : 1)

#define GAME_GRAPHICS_RENDER(gameGraphics, rabbits, rabbcount) gameGraphics->m_rabbitsToRender = rabbits; \
gameGraphics->m_rabbitCount = rabbcount; \
gameGraphics->m_selected = g_gameConfig.selected; \
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

    Rabbit* m_rabbitsToRender;
    int m_rabbitCount;
    /*Obstacle** ObstaclesToRender;
    int ObstacleCount;*/

    Rabbit* m_selected;

    SpriteGroup* m_hoverSprite;
    SpriteGroup* m_hoverSpriteFox;
    SpriteGroup* m_holeSprite;
    SpriteGroup* m_mastermindSprite;
    SpriteGroup* m_oupiGoupiSprite;
    SpriteGroup* m_minusFiveSprite;
    SpriteGroup* m_explosionSprite;
    SpriteGroup* m_lowQualityCatSprite;

    SAudio* m_candyBoomAudio;
    SAudio* m_homeRunAudio;
    SAudio* m_earthquakeAudio;
    SAudio* m_undertaleBoomAudio;
    SAudio* m_stoneSlideAudio;
    SAudio* m_punchesAudio[5];
    SAudio* m_explosionAudio;

    Vec2 m_lastWindowPosition;

    bool m_enabled;
    bool m_leftPressed;

    float m_fiscGuyScale;
    int m_lastFiscPos;
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
