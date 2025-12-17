/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "game/game_graphics.h"
#include "common/assets.h"
#include "common/common.h"
#include "game/input.h"
#include "game/scene.h"
#include "game/game_config.h"
#include "game/game_editor.h"
#include "game/core/game_core.h"
#include "game/game_loader.h"

GameGraphics* GameGraphics_create(Scene* scene)
{
    GameGraphics* self = (GameGraphics*)calloc(1, sizeof(GameGraphics));
    AssertNew(self);

    self->m_scene = scene;
    self->m_padding = Vec2_set(0.0f, 0.0f);
    self->m_spacing = Vec2_set(0.1f, 0.1f);
    self->m_gridAABB.lower = Vec2_add(Vec2_set(-4.f, -4.f), Vec2_set(8.0f, 4.5f));
    self->m_gridAABB.upper = Vec2_add(Vec2_set(+4.f, +4.f), Vec2_set(8.0f, 4.5f));
    self->m_enabled = false;
    self->CandyBoomAudio = AudioManager_loadWav(g_gameConfig.Audio, "bang_effect.wav", "bang-effect");

    AssetManager* assets = Scene_getAssetManager(scene);
    SpriteSheet* spriteSheet = AssetManager_getSpriteSheet(assets, SPRITE_UI_BASE);
    AssertNew(spriteSheet);
    self->HoverSprite = SpriteSheet_getGroupByName(spriteSheet, "border");
    AssertNew(self->HoverSprite);

    spriteSheet = AssetManager_getSpriteSheet(assets, SPRITE_GAME);
    self->HoleSprite = SpriteSheet_getGroupByName(spriteSheet, "rabbit-hole");

    spriteSheet = AssetManager_getSpriteSheet(assets, SPRITE_UI_SELECT_BOX);
    AssertNew(spriteSheet);
    self->HoverSpriteFox = SpriteSheet_getGroupByName(spriteSheet, "select_box");
    AssertNew(self->HoverSpriteFox);

    self->MastermindSprite = SpriteSheet_getGroupByName(AssetManager_getSpriteSheet(assets, SPRITE_MASTERMIND), "mastermind");

    return self;
}

void GameGraphics_destroy(GameGraphics* self)
{
    if (!self) return;
    free(self);
}

static void GameGraphics_updateCells(GameGraphics* self)
{
    float totalPaddingX = self->m_padding.x * 2.f;
    float totalPaddingY = self->m_padding.y * 2.f;
    float totalSpacingX = self->m_spacing.x * (GAME_GRID_SIZE - 1);
    float totalSpacingY = self->m_spacing.y * (GAME_GRID_SIZE - 1);
    Vec2 gridSize = AABB_getSize(&(self->m_gridAABB));
    float cellW = (gridSize.x - totalPaddingX - totalSpacingX) / GAME_GRID_SIZE;
    float cellH = (gridSize.y - totalPaddingY - totalSpacingY) / GAME_GRID_SIZE;

    for (int i = 0; i < GAME_GRID_SIZE; i++)
    {
        for (int j = 0; j < GAME_GRID_SIZE; j++)
        {
            float cellX = self->m_gridAABB.lower.x + self->m_padding.x + j * (cellW + self->m_spacing.x);
            float cellY = self->m_gridAABB.lower.y + self->m_padding.y + i * (cellH + self->m_spacing.y);
            self->m_cells[GAME_GRID_SIZE - 1 - i][j].lower = Vec2_set(cellX, cellY);
            self->m_cells[GAME_GRID_SIZE - 1 - i][j].upper = Vec2_set(cellX + cellW, cellY + cellH);
        }
    }
}

void GameGraphics_update(GameGraphics* self)
{
    assert(self && "self must not be NULL");
    Scene* scene = self->m_scene;
    Input* input = Scene_getInput(scene);
    Camera* camera = Scene_getCamera(scene);

    GameGraphics_updateCells(self);

    Vec2 mouseWorldPos = { 0 };
    Camera_viewToWorld(camera, input->mouse.position.x, input->mouse.position.y, &mouseWorldPos);

    //printf("%f %f\n", input->mouse.position.x, input->mouse.position.y);

    if (self->m_enabled == false)
    {
        return;
    }

    if (input->mouse.rightPressed && g_gameConfig.isEditing)
    {
        for (int i = 0; i < GAME_GRID_SIZE; i++)
        {
            for (int j = 0; j < GAME_GRID_SIZE; j++)
            {
                AABB* cellAABB = &(self->m_cells[i][j]);

                if (AABB_containsPoint(cellAABB, mouseWorldPos))
                {
                    Rabbit* l_Aimed = NULL;
                    if (GameCore_isAimingRabbit(g_gameConfig.Core, j, i, &l_Aimed))
                    {
                        if (l_Aimed)
                        {
                            GameCore_deletePiece(g_gameConfig.Core, l_Aimed);
                        }
                    }
                }
            }
        }
    }

    if (input->mouse.leftPressed || g_gameEditor.AddingObject)
    {
        for (int i = 0; i < GAME_GRID_SIZE; i++)
        {
            for (int j = 0; j < GAME_GRID_SIZE; j++)
            {
                AABB* cellAABB = &(self->m_cells[i][j]);
                if (AABB_containsPoint(cellAABB, mouseWorldPos))
                {
                    if (g_gameConfig.State != PLAYING)
                    {

                        if (g_gameConfig.isEditing && input->mouse.leftPressed)
                        {
                            for (int x = MAX_RABBITS; x < MAX_RABBITS + FOX_COUNT; x++)
                            {
                                Rabbit* l_Fox = &(g_gameConfig.Core->Rabbits[x]);

                                if (l_Fox->CellX == j && l_Fox->CellY == i)
                                {
                                    printf("directioning\n");
                                    do
                                    {
                                        if (l_Fox->Direction == RABBIT_WEST)
                                        {
                                            l_Fox->Direction = RABBIT_NORTH;
                                        }
                                        else
                                        {
                                            l_Fox->Direction += 1;
                                        }
                                    } while (!Rabbit_canBePlaced(g_gameConfig.Core, l_Fox));
                                }
                            }
                        }

                        if (g_gameEditor.AddingObject)
                        {
                            g_gameEditor.AddingObject->CellX = j;
                            g_gameEditor.AddingObject->CellY = i;

                            if (input->mouse.leftPressed)
                            {
                                if (!Rabbit_canBePlaced(g_gameConfig.Core, g_gameEditor.AddingObject))
                                {
                                    printf("\nCannot add here\n");
                                    break;
                                }

                                int indexToAdd = 0;
                                switch (g_gameEditor.AddingObject->Type)
                                {
                                case RABBIT:
                                    if (RABBIT_COUNT == MAX_RABBITS)
                                    {
                                        indexToAdd = -1;
                                        break;
                                    }

                                    indexToAdd = RABBIT_COUNT;
                                    RABBIT_COUNT += 1;
                                    break;
                                case FOX:
                                    if (FOX_COUNT == MAX_FOXES)
                                    {
                                        indexToAdd = -1;
                                        break;
                                    }

                                    indexToAdd = MAX_RABBITS + FOX_COUNT;
                                    FOX_COUNT += 1;
                                    break;
                                case MUSHROOM:
                                    if (MUSHROOM_COUNT == MAX_MUSHROOMS)
                                    {
                                        indexToAdd = -1;
                                        break;
                                    }

                                    indexToAdd = MAX_RABBITS + MAX_FOXES + MUSHROOM_COUNT;
                                    MUSHROOM_COUNT += 1;
                                    break;
                                default: break;
                                }

                                if (indexToAdd < 0)
                                {
                                    printf("\nCannot add more of this object\n");
                                    break;
                                }

                                g_gameConfig.Core->Rabbits[indexToAdd] = *g_gameEditor.AddingObject;
                                g_gameEditor.AddingObject = NULL;
                            }
                        }
                    }

                    self->m_selectedRowIndex = i;
                    self->m_selectedColIndex = j;
                }
            }
        }
    }
}

#define MASTERMIND_ANIM_FADE_DUR 4.f
#define MASTERMIND_ANIM_EFFECT_DUR 6.f
#define MASTERMIND_OUT_FADE_DUR 3.f
#define MASTERMIND_APPLY_DELAY 4.f
#define MASTERMIND_LEVEL "../../../mastermind_level.txt"

void GameGraphics_render(GameGraphics* self)
{
    assert(self && "self must not be NULL");

    Scene* scene = self->m_scene;
    Camera* camera = Scene_getCamera(scene);
    float scale = Camera_getWorldToViewScale(camera);

    SDL_FRect rect = { 0 };
    for (int i = 0; i < GAME_GRID_SIZE; i++)
    {
        for (int j = 0; j < GAME_GRID_SIZE; j++)
        {
            AABB* cellAABB = &(self->m_cells[i][j]);
            rect.x = Camera_worldToViewX(camera, cellAABB->lower.x);
            rect.y = Camera_worldToViewY(camera, cellAABB->upper.y);
            rect.w = (cellAABB->upper.x - cellAABB->lower.x) * scale;
            rect.h = (cellAABB->upper.y - cellAABB->lower.y) * scale;

            bool isSelected = false;

            SDL_Color color = isSelected ? g_colors.orange9 : g_colors.gray8;

            SDL_SetRenderDrawColor(g_renderer, color.r, color.g, color.b, 255);
            SDL_RenderFillRect(g_renderer, &rect);

            if (i == GAME_GRID_SIZE / 2 && j == GAME_GRID_SIZE / 2 || i == 0 && j == 0 || j == 0 && i == GAME_GRID_SIZE - 1 || i == 0 && j == GAME_GRID_SIZE - 1 || (i == GAME_GRID_SIZE - 1 && j == GAME_GRID_SIZE - 1))
            {
                SpriteGroup_render(self->HoleSprite, 0, &rect, Vec2_anchor_north_west, 1.0f);
            }
        }
    }

    bool l_Cond = g_gameConfig.isEditing && g_gameEditor.AddingObject;

    for (int x = 0; x < self->RabbitCount + l_Cond; x++)
    {
        Rabbit* l_Rabb;
        if (x == self->RabbitCount)
        {
            l_Rabb = g_gameEditor.AddingObject;
        }
        else
        {
            l_Rabb = &self->RabbitsToRender[x];
        }

        if (!l_Rabb) continue;

        int l_CellX = l_Rabb->CellX;
        int l_CellY = l_Rabb->CellY;

        SDL_FRect l_Rect = { 0 };
        AABB* l_RectCell = &(self->m_cells[l_CellY][l_CellX]);

        l_Rect.x = Camera_worldToViewX(camera, l_RectCell->lower.x);
        l_Rect.y = Camera_worldToViewY(camera, l_RectCell->upper.y);
        l_Rect.w = (l_RectCell->upper.x - l_RectCell->lower.x) * scale;
        l_Rect.h = (l_RectCell->upper.y - l_RectCell->lower.y) * scale;

        Vec2 l_Anchor = Vec2_anchor_north_west;
        float l_Angle = 0.f;
        if (l_Rabb->Type == FOX)
        {
            l_Rect.h *= 2;

            Rabbit_getAnchorAngAngleFromDirection(l_Rabb->Direction, &l_Anchor, &l_Angle);
        }

        SpriteGroup_setColorModFloat(l_Rabb->RabbitSprite, 1, 1, 1);
        if (x == self->RabbitCount)
        {
            SpriteGroup_setOpacityFloat(l_Rabb->RabbitSprite, .5f);
        }
        else
        {
            SpriteGroup_setOpacityFloat(l_Rabb->RabbitSprite, 1);
        }
        SpriteGroup_renderRotated(l_Rabb->RabbitSprite, l_Rabb == (self->Selected), &l_Rect, l_Anchor, l_Angle, 0.9f);

        if ((l_Rabb == (self->Selected) || self->Selected ? l_Rabb->CellX == self->Selected->CellX && l_Rabb->CellY == self->Selected->CellY : false) && g_gameConfig.State == PLAYING)
        {
            SpriteGroup_setColorModFloat(l_Rabb->Type == FOX ? self->HoverSpriteFox : self->HoverSprite, 1.f, 209.f / 255.f, 145.f / 255.f);

            SpriteGroup_renderRotated(l_Rabb->Type == FOX ? self->HoverSpriteFox : self->HoverSprite, 0, &l_Rect, l_Anchor, l_Angle, 1.f);
        }
    }

    if (g_gameConfig.State == GAMBLING && g_gameConfig.GamblingResult == CANDY)
    {
        Vec2 l_LastCandyPos = g_gameConfig.CandyPos;

        SpriteGroup* l_Candy = SpriteSheet_getGroupByName(AssetManager_getSpriteSheet(scene->m_assets, SPRITE_CANDY), "candy");
        SDL_FRect l_Rec;
        l_Rec.x = g_gameConfig.CandyPos.x;
        l_Rec.y = g_gameConfig.CandyPos.y;
        l_Rec.w = 200.0;
        l_Rec.h = 200.f;

        g_gameConfig.CandyAcc = Vec2_add(g_gameConfig.CandyAcc, Vec2_set(0, (9.81f * Timer_getDelta(g_time) * 2)));
        g_gameConfig.CandyPos = Vec2_add(g_gameConfig.CandyPos, Vec2_scale(g_gameConfig.CandyAcc, 20*Timer_getDelta(g_time)));

        if (g_gameConfig.CandyPos.y >= HD_HEIGHT && l_LastCandyPos.y < HD_HEIGHT)
        {
            AudioManager_play(g_gameConfig.Audio, self->CandyBoomAudio);
        }

        if (g_gameConfig.CandyPos.y >= HD_HEIGHT)
        {
            g_gameConfig.CandyPos.y = HD_HEIGHT;
            int wx;
            int wy;
            SDL_GetWindowPosition(g_window, &wx, &wy);

            //printf("%d %d /=> %f %f\n", wx, wy, wx + ceil(g_gameConfig.CandyAcc.x * 20 * Timer_getDelta(g_time)), wy + ceil(g_gameConfig.CandyAcc.y * 20 * Timer_getDelta(g_time)));
            SDL_SetWindowPosition(g_window, wx + ceil(g_gameConfig.CandyAcc.x * 20 * Timer_getDelta(g_time)), wy + ceil(g_gameConfig.CandyAcc.y * 20 * Timer_getDelta(g_time)));

            if (wy >= 5000)
            {
                SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, "IMPORTANT INFO", "you lost btw", g_window);
                exit(0);
            }
        }

        SpriteGroup_render(l_Candy, 0, &l_Rec, Vec2_anchor_center, 5.f);
    }

    if (g_gameConfig.State == GAMBLING && g_gameConfig.GamblingResult == MASTERMIND)
    {
        float l_LastTime = g_gameConfig.GamblingAnimTime;

        g_gameConfig.GamblingAnimTime += Timer_getDelta(g_time);


        if (g_gameConfig.GamblingAnimTime < MASTERMIND_ANIM_FADE_DUR)
        {
            SDL_SetRenderDrawColor(g_renderer, 0, 0, 0, Float_clamp((g_gameConfig.GamblingAnimTime / (MASTERMIND_ANIM_FADE_DUR - 1)) * 150, 0, 150));
            SDL_RenderFillRect(g_renderer, NULL);
        }
        else if (g_gameConfig.GamblingAnimTime >= MASTERMIND_ANIM_FADE_DUR && g_gameConfig.GamblingAnimTime < MASTERMIND_ANIM_FADE_DUR + MASTERMIND_ANIM_EFFECT_DUR)
        {
            SpriteGroup* l_Master = self->MastermindSprite;

            SDL_SetRenderDrawColor(g_renderer, 0, 0, 0, 150);
            SDL_RenderFillRect(g_renderer, NULL);

            float l_FixedTime = g_gameConfig.GamblingAnimTime - MASTERMIND_ANIM_FADE_DUR;

            SDL_FRect l_Rec;
            l_Rec.x = HD_WIDTH / 2;
            l_Rec.y = HD_HEIGHT / 2;
            l_Rec.w = 200;
            l_Rec.h = 200;

            SpriteGroup_setOpacityFloat(l_Master, 1);
            SpriteGroup_render(l_Master, 0, &l_Rec, Vec2_anchor_center, 1.f);

            l_Rec.w = 200 + 200 * (l_FixedTime / (MASTERMIND_ANIM_EFFECT_DUR / 6));
            l_Rec.h = 200 + 200 * (l_FixedTime / (MASTERMIND_ANIM_EFFECT_DUR / 6));

            SpriteGroup_setOpacityFloat(l_Master, 0.99f - (l_FixedTime / (MASTERMIND_ANIM_EFFECT_DUR / 6)));
            SpriteGroup_render(l_Master, 0, &l_Rec, Vec2_anchor_center, 1.f);
        }
        else if (g_gameConfig.GamblingAnimTime >= MASTERMIND_ANIM_FADE_DUR + MASTERMIND_ANIM_EFFECT_DUR && g_gameConfig.GamblingAnimTime < MASTERMIND_ANIM_FADE_DUR + MASTERMIND_ANIM_EFFECT_DUR + MASTERMIND_OUT_FADE_DUR)
        {
            float l_FixedTime = g_gameConfig.GamblingAnimTime - MASTERMIND_ANIM_FADE_DUR - MASTERMIND_ANIM_EFFECT_DUR;

            SDL_SetRenderDrawColor(g_renderer, 0, 0, 0, 150 - (l_FixedTime / MASTERMIND_OUT_FADE_DUR) * 150);
            SDL_RenderFillRect(g_renderer, NULL);

            SDL_FRect l_Rec;
            l_Rec.x = HD_WIDTH / 2;
            l_Rec.y = HD_HEIGHT / 2;
            l_Rec.w = 200;
            l_Rec.h = 200;

            SpriteGroup* l_Master = self->MastermindSprite;

            SpriteGroup_setOpacityFloat(l_Master, 1 - (l_FixedTime / MASTERMIND_OUT_FADE_DUR));
            SpriteGroup_render(l_Master, 0, &l_Rec, Vec2_anchor_center, 1.f);
        }
        else if (g_gameConfig.GamblingAnimTime >= MASTERMIND_ANIM_FADE_DUR + MASTERMIND_ANIM_EFFECT_DUR + MASTERMIND_OUT_FADE_DUR + MASTERMIND_APPLY_DELAY)
        {
            float l_Remaining = g_gameConfig.Remaining;

            GameLoader_loadGame(MASTERMIND_LEVEL, true);

            g_gameConfig.Remaining = l_Remaining;
            g_gameConfig.State = PLAYING;
        }

    }
}
