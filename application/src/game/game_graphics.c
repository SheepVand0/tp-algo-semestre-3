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
    self->m_candyBoomAudio = AudioManager_loadWav(g_gameConfig.audio, "bang_effect.wav", "bang-effect");
    self->m_homeRunAudio = AudioManager_loadWav(g_gameConfig.audio, "home_run.wav", "home-run");
    self->m_earthquakeAudio = AudioManager_loadWav(g_gameConfig.audio, "earthquake.wav", "earthquake");
    self->m_undertaleBoomAudio = AudioManager_loadWav(g_gameConfig.audio, "undertale_boom_sound.wav", "undertale-boom-sound");
    self->m_explosionAudio = AudioManager_loadWav(g_gameConfig.audio, "explosion.wav", "explosion");
    self->m_stoneSlideAudio = AudioManager_loadWav(g_gameConfig.audio, "stone_slide.wav", "explosion");

    for (int x = 0; x < 5; x++)
    {
        char* l_Punch = "punchN.wav";

        sprintf(l_Punch, "punch%d.wav", x);

        self->m_punchesAudio[x] = AudioManager_loadWav(g_gameConfig.audio, l_Punch, l_Punch);
    }

    AssetManager* assets = Scene_getAssetManager(scene);
    SpriteSheet* spriteSheet = AssetManager_getSpriteSheet(assets, SPRITE_UI_BASE);
    AssertNew(spriteSheet);
    self->m_hoverSprite = SpriteSheet_getGroupByName(spriteSheet, "border");
    AssertNew(self->m_hoverSprite);

    spriteSheet = AssetManager_getSpriteSheet(assets, SPRITE_GAME);
    self->m_holeSprite = SpriteSheet_getGroupByName(spriteSheet, "rabbit-hole");

    spriteSheet = AssetManager_getSpriteSheet(assets, SPRITE_UI_SELECT_BOX);
    AssertNew(spriteSheet);
    self->m_hoverSpriteFox = SpriteSheet_getGroupByName(spriteSheet, "select_box");
    AssertNew(self->m_hoverSpriteFox);

    self->m_mastermindSprite = SpriteSheet_getGroupByName(AssetManager_getSpriteSheet(assets, SPRITE_MASTERMIND), "mastermind");
    self->m_oupiGoupiSprite = SpriteSheet_getGroupByName(AssetManager_getSpriteSheet(assets, SPRITE_OUPI_GOUPI), "oupigoupi");
    self->m_minusFiveSprite = SpriteSheet_getGroupByName(AssetManager_getSpriteSheet(assets, SPRITE_MINUS_FIVE), "number");
    self->m_explosionSprite = SpriteSheet_getGroupByName(AssetManager_getSpriteSheet(assets, SPRITE_EXPLOSION), "explosion");
    self->m_lowQualityCatSprite = SpriteSheet_getGroupByName(AssetManager_getSpriteSheet(assets, SPRITE_LOW_QUALITY_CAT), "cat");

    self->m_fiscGuyScale = 1.f;
    self->m_lastFiscPos = -1;

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
                    if (GameCore_isAimingRabbit(g_gameConfig.core, j, i, &l_Aimed))
                    {
                        if (l_Aimed)
                        {
                            GameCore_deletePiece(g_gameConfig.core, l_Aimed);
                        }
                    }
                }
            }
        }
    }

    self->m_leftPressed = input->mouse.leftDown;

    if (self->m_leftPressed && g_gameConfig.state == GAMBLING && g_gameConfig.gamblingResult == CANDY && !g_gameConfig.candyHasTakenOver)
    {
        g_gameConfig.candyWaitingForLeftClick = false;
        Vec2 l_LastPos = g_gameConfig.candyPos;

        g_gameConfig.candyPos = Vec2_set(input->mouse.position.x, input->mouse.position.y);
        g_gameConfig.candyVel = Vec2_scale(Vec2_sub(g_gameConfig.candyPos, l_LastPos), 1);
    }

    g_gameConfig.candyHasTakenOver = !self->m_leftPressed && !g_gameConfig.candyWaitingForLeftClick;


    if (input->mouse.leftPressed || g_gameEditor.AddingObject)
    {
        for (int i = 0; i < GAME_GRID_SIZE; i++)
        {
            for (int j = 0; j < GAME_GRID_SIZE; j++)
            {
                AABB* cellAABB = &(self->m_cells[i][j]);
                if (AABB_containsPoint(cellAABB, mouseWorldPos))
                {
                    if (g_gameConfig.state != PLAYING)
                    {
                        if (g_gameConfig.isEditing && input->mouse.leftPressed)
                        {
                            for (int x = MAX_RABBITS; x < MAX_RABBITS + FOX_COUNT; x++)
                            {
                                Rabbit* l_Fox = &(g_gameConfig.core->Rabbits[x]);

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
                                    } while (!Rabbit_canBePlaced(g_gameConfig.core, l_Fox));
                                }
                            }
                        }

                        if (g_gameEditor.AddingObject)
                        {
                            g_gameEditor.AddingObject->CellX = j;
                            g_gameEditor.AddingObject->CellY = i;

                            if (input->mouse.leftPressed)
                            {
                                if (!Rabbit_canBePlaced(g_gameConfig.core, g_gameEditor.AddingObject))
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

                                g_gameConfig.core->Rabbits[indexToAdd] = *g_gameEditor.AddingObject;
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
#define MASTERMIND_APPLY_DELAY 6.f
#define MASTERMIND_LEVEL "../../../mastermind_level.txt"

#define OUPI_GOUPI_ENTRANCE_DURATION 14.f
#define OUPI_GOUPI_PRE_PUNCH_DURATION 4.f
#define OUPI_GOUPI_PUNCH_DURATION 5.f
#define OUPI_GOUPI_PUNCH_LENGTH 0.8f
#define OUPI_GOUPI_EXPLOSION_PRE_DURATION 2.f
#define OUPI_GOUPI_EXPLOSION_DURATION 1.f
#define OUPI_GOUPI_EXPLOSION_POST_DURATION 1.f

#define FISC_ENTRANCE 15.f
#define FISC_PRE_MOVE_DUR 4.f
#define FISC_POST_MOVE_DUR 4.f
#define FISC_EXIT 15.f

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

            if (g_gameConfig.state == GAMBLING && g_gameConfig.gamblingResult == FISC_GUY)
            {
                if (g_gameConfig.solveNextSol[0].CellX == j && g_gameConfig.solveNextSol[0].CellY == i)
                    g_gameConfig.fiscTargetPos = Vec2_set(rect.x, rect.y + (rect.h / 2));

                if (g_gameConfig.solveNextSol[1].CellX == j && g_gameConfig.solveNextSol[1].CellY == i)
                    g_gameConfig.fiscNextMovePos = Vec2_set(rect.x, rect.y + (rect.h / 2));
            }

            bool isSelected = false;

            SDL_Color color = isSelected ? g_colors.orange9 : g_colors.gray8;

            SDL_SetRenderDrawColor(g_renderer, color.r, color.g, color.b, 255);
            SDL_RenderFillRect(g_renderer, &rect);

            if (i == GAME_GRID_SIZE / 2 && j == GAME_GRID_SIZE / 2 || i == 0 && j == 0 || j == 0 && i == GAME_GRID_SIZE - 1 || i == 0 && j == GAME_GRID_SIZE - 1 || (i == GAME_GRID_SIZE - 1 && j == GAME_GRID_SIZE - 1))
            {
                SpriteGroup_render(self->m_holeSprite, 0, &rect, Vec2_anchor_north_west, 1.0f);
            }
        }
    }

    bool l_Cond = g_gameConfig.isEditing && g_gameEditor.AddingObject;

    for (int x = 0; x < self->m_rabbitCount + l_Cond; x++)
    {
        Rabbit* l_Rabb;
        if (x == self->m_rabbitCount)
        {
            l_Rabb = g_gameEditor.AddingObject;
        }
        else
        {
            l_Rabb = &self->m_rabbitsToRender[x];
        }

        if (!l_Rabb) continue;

        if (l_Rabb->Type == NONE) continue;

        

        int l_CellX = l_Rabb->CellX;
        int l_CellY = l_Rabb->CellY;

        if (l_CellX < 0 || l_CellX >= GAME_GRID_SIZE || l_CellY < 0 || l_CellY >= GAME_GRID_SIZE)
        {
            
            continue;
        }

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
        if (x == self->m_rabbitCount)
        {
            SpriteGroup_setOpacityFloat(l_Rabb->RabbitSprite, .5f);
        }
        else
        {
            SpriteGroup_setOpacityFloat(l_Rabb->RabbitSprite, 1);
        }
        //l_Rect.x += l_Rect.w / 2;

        SpriteGroup_renderRotated(l_Rabb->RabbitSprite, l_Rabb == (self->m_selected), &l_Rect, l_Anchor, l_Angle, 0.9f);

        if ((l_Rabb == (self->m_selected) || self->m_selected ? l_Rabb->CellX == self->m_selected->CellX && l_Rabb->CellY == self->m_selected->CellY : false) && g_gameConfig.state == PLAYING)
        {
            SpriteGroup_setColorModFloat(l_Rabb->Type == FOX ? self->m_hoverSpriteFox : self->m_hoverSprite, 1.f, 209.f / 255.f, 145.f / 255.f);

            SpriteGroup_renderRotated(l_Rabb->Type == FOX ? self->m_hoverSpriteFox : self->m_hoverSprite, 0, &l_Rect, l_Anchor, l_Angle, 1.f);
        }
    }

    if (g_gameConfig.state == GAMBLING && g_gameConfig.gamblingResult == CANDY)
    {
        Vec2 l_LastCandyPos = g_gameConfig.candyPos;

        SpriteGroup* l_Candy = SpriteSheet_getGroupByName(AssetManager_getSpriteSheet(scene->m_assets, SPRITE_CANDY), "candy");
        SDL_FRect l_Rec;
        l_Rec.x = g_gameConfig.candyPos.x;
        l_Rec.y = g_gameConfig.candyPos.y;
        l_Rec.w = 200.f;
        l_Rec.h = 200.f;

        if (g_gameConfig.candyHasTakenOver)
        {
            g_gameConfig.candyVel = Vec2_add(g_gameConfig.candyVel, Vec2_set(0, (9.81f * Timer_getDelta(g_time) * 2)));
            g_gameConfig.candyPos = Vec2_add(g_gameConfig.candyPos, Vec2_scale(g_gameConfig.candyVel, 20 * Timer_getDelta(g_time)));
        }

        float checkPos = (HD_HEIGHT - (l_Rec.h / 2));
        float checkPosX = (HD_WIDTH) - (l_Rec.w / 2);


        if (g_gameConfig.candyPos.y >= checkPos && l_LastCandyPos.y < checkPos)
        {
            AudioManager_play(g_gameConfig.audio, self->m_candyBoomAudio);
        }

        if (g_gameConfig.candyPos.x >= checkPosX && l_LastCandyPos.x < checkPosX || (g_gameConfig.candyPos.x <= 0 && l_LastCandyPos.x > 0))
        {
            AudioManager_play(g_gameConfig.audio, self->m_candyBoomAudio);
        }

        if ((g_gameConfig.candyPos.y >= checkPos || g_gameConfig.candyPos.y <= 0 || (g_gameConfig.candyPos.x <= 0 || g_gameConfig.candyPos.x >= checkPosX)) && g_gameConfig.candyHasTakenOver)
        {
            g_gameConfig.candyPos.y = Float_clamp(g_gameConfig.candyPos.y, 0, checkPos);
            g_gameConfig.candyPos.x = Float_clamp(g_gameConfig.candyPos.x, 0, checkPosX);
            int wx;
            int wy;
            SDL_GetWindowPosition(g_window, &wx, &wy);

            float posX = 0;
            if (g_gameConfig.candyPos.x <= 0 || g_gameConfig.candyPos.x >= checkPosX)
                posX = ceil(g_gameConfig.candyVel.x * 15 * Timer_getDelta(g_time));
            float posY = 0;
            if (g_gameConfig.candyPos.y >= checkPos || g_gameConfig.candyPos.y <= 0)
                posY = ceil(g_gameConfig.candyVel.y * 15 * Timer_getDelta(g_time));

            //printf("%d %d /=> %f %f\n", wx, wy, wx + ceil(g_gameConfig.CandyAcc.x * 20 * Timer_getDelta(g_time)), wy + ceil(g_gameConfig.CandyAcc.y * 20 * Timer_getDelta(g_time)));
            SDL_SetWindowPosition(g_window, wx + posX, wy + posY);

            if (wy >= 5000)
            {
                SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, "IMPORTANT INFO", "you lost btw", g_window);
                exit(0);
            }
        }

        SpriteGroup_render(l_Candy, 0, &l_Rec, Vec2_anchor_center, 5.f);
    }

    if (g_gameConfig.state == GAMBLING && g_gameConfig.gamblingResult == MASTERMIND)
    {
        float l_LastTime = g_gameConfig.gamblingAnimTime;

        g_gameConfig.gamblingAnimTime += Timer_getDelta(g_time);


        if (g_gameConfig.gamblingAnimTime < MASTERMIND_ANIM_FADE_DUR)
        {
            SDL_SetRenderDrawColor(g_renderer, 0, 0, 0, Float_clamp((g_gameConfig.gamblingAnimTime / (MASTERMIND_ANIM_FADE_DUR - 1)) * 150, 0, 150));
            SDL_RenderFillRect(g_renderer, NULL);
        }
        else if (g_gameConfig.gamblingAnimTime >= MASTERMIND_ANIM_FADE_DUR && g_gameConfig.gamblingAnimTime < MASTERMIND_ANIM_FADE_DUR + MASTERMIND_ANIM_EFFECT_DUR)
        {
            if (l_LastTime < MASTERMIND_ANIM_FADE_DUR)
            {
                AudioManager_play(g_gameConfig.audio, self->m_homeRunAudio);
            }

            SpriteGroup* l_Master = self->m_mastermindSprite;

            SDL_SetRenderDrawColor(g_renderer, 0, 0, 0, 150);
            SDL_RenderFillRect(g_renderer, NULL);

            float l_FixedTime = g_gameConfig.gamblingAnimTime - MASTERMIND_ANIM_FADE_DUR;

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
        else if (g_gameConfig.gamblingAnimTime >= MASTERMIND_ANIM_FADE_DUR + MASTERMIND_ANIM_EFFECT_DUR && g_gameConfig.gamblingAnimTime < MASTERMIND_ANIM_FADE_DUR + MASTERMIND_ANIM_EFFECT_DUR + MASTERMIND_OUT_FADE_DUR)
        {
            float l_FixedTime = g_gameConfig.gamblingAnimTime - MASTERMIND_ANIM_FADE_DUR - MASTERMIND_ANIM_EFFECT_DUR;

            SDL_SetRenderDrawColor(g_renderer, 0, 0, 0, 150 - (l_FixedTime / MASTERMIND_OUT_FADE_DUR) * 150);
            SDL_RenderFillRect(g_renderer, NULL);

            SDL_FRect l_Rec;
            l_Rec.x = HD_WIDTH / 2;
            l_Rec.y = HD_HEIGHT / 2;
            l_Rec.w = 200;
            l_Rec.h = 200;

            SpriteGroup* l_Master = self->m_mastermindSprite;

            SpriteGroup_setOpacityFloat(l_Master, 1 - (l_FixedTime / MASTERMIND_OUT_FADE_DUR));
            SpriteGroup_render(l_Master, 0, &l_Rec, Vec2_anchor_center, 1.f);
        }
        else if (g_gameConfig.gamblingAnimTime >= MASTERMIND_ANIM_FADE_DUR + MASTERMIND_ANIM_EFFECT_DUR + MASTERMIND_OUT_FADE_DUR && g_gameConfig.gamblingAnimTime < MASTERMIND_ANIM_FADE_DUR + MASTERMIND_ANIM_EFFECT_DUR + MASTERMIND_OUT_FADE_DUR + MASTERMIND_APPLY_DELAY)
        {
            if (l_LastTime < MASTERMIND_ANIM_FADE_DUR + MASTERMIND_ANIM_EFFECT_DUR + MASTERMIND_OUT_FADE_DUR)
            {
                int wx;
                int wy;
                SDL_GetWindowPosition(g_window, &wx, &wy);
                self->m_lastWindowPosition = Vec2_set(wx, wy);
                AudioManager_play(g_gameConfig.audio, self->m_earthquakeAudio);
            }

            SDL_SetWindowPosition(g_window, self->m_lastWindowPosition.x + (rand() % 10) - 5, self->m_lastWindowPosition.y + (rand() % 10) - 5);
        }
        else if (g_gameConfig.gamblingAnimTime >= MASTERMIND_ANIM_FADE_DUR + MASTERMIND_ANIM_EFFECT_DUR + MASTERMIND_OUT_FADE_DUR + MASTERMIND_APPLY_DELAY)
        {
            if (l_LastTime < MASTERMIND_ANIM_FADE_DUR + MASTERMIND_ANIM_EFFECT_DUR + MASTERMIND_OUT_FADE_DUR + MASTERMIND_APPLY_DELAY)
            {
                SDL_SetWindowPosition(g_window, self->m_lastWindowPosition.x, self->m_lastWindowPosition.y);
                AudioManager_stopAll(g_gameConfig.audio);
                AudioManager_play(g_gameConfig.audio, self->m_undertaleBoomAudio);
            }

            float l_Remaining = g_gameConfig.remaining;

            GameLoader_loadGame(MASTERMIND_LEVEL, true);

            g_gameConfig.remaining = l_Remaining;
            g_gameConfig.state = PLAYING;
        }

    }

    if (g_gameConfig.state == GAMBLING && g_gameConfig.gamblingResult == OUPI_GOUPI)
    {
        float l_LastTime = g_gameConfig.gamblingAnimTime;

        g_gameConfig.gamblingAnimTime += Timer_getDelta(g_time);
        float time = g_gameConfig.gamblingAnimTime;

        SDL_FRect l_Rec;
        l_Rec.x = 0;
        l_Rec.y = 16.f;
        l_Rec.w = 120.f;
        l_Rec.h = 120.f;

        //600

        if (g_gameConfig.gamblingAnimTime < OUPI_GOUPI_ENTRANCE_DURATION)
        {
            if (l_LastTime == 0)
            {
                // Play drag sound
                AudioManager_play(g_gameConfig.audio, self->m_stoneSlideAudio);
            }

            l_Rec.x = 600 * (g_gameConfig.gamblingAnimTime / OUPI_GOUPI_ENTRANCE_DURATION);
        }
        else if (time < OUPI_GOUPI_ENTRANCE_DURATION + OUPI_GOUPI_PRE_PUNCH_DURATION)
        {
            if (l_LastTime < OUPI_GOUPI_ENTRANCE_DURATION)
            {
                // Stop drag sound
                AudioManager_stopAll(g_gameConfig.audio);
            }

            l_Rec.x = 600;
        }
        else if (time < OUPI_GOUPI_ENTRANCE_DURATION + OUPI_GOUPI_PRE_PUNCH_DURATION + OUPI_GOUPI_PUNCH_DURATION)
        {
            float l_Mod = ModFloat(time, OUPI_GOUPI_PUNCH_LENGTH);
            float l_TimeModded = time - l_Mod;

            float l_LastMod = ModFloat(l_LastTime, OUPI_GOUPI_PUNCH_LENGTH);
            float l_LastTimeModded = l_LastTime - l_LastMod;

            if (l_TimeModded != l_LastTimeModded)
            {
                // Play punch sound
                AudioManager_play(g_gameConfig.audio, self->m_punchesAudio[rand() % 5]);
                g_gameConfig.remaining -= 5;
            }

            SpriteGroup* l_minusFive = self->m_minusFiveSprite;

            SpriteGroup_setColorModFloat(l_minusFive, 1, 0.2f, 0.2f);
            SpriteGroup_setOpacityFloat(l_minusFive, 1 - (l_Mod / OUPI_GOUPI_PUNCH_LENGTH));
            SDL_FRect l_NRec;
            l_NRec.x = 670;
            l_NRec.y = 20 - (l_Mod / OUPI_GOUPI_PUNCH_LENGTH) * 20;
            l_NRec.w = 40.f;
            l_NRec.h = 40.f;

            SpriteGroup_render(l_minusFive, 0, &l_NRec, Vec2_anchor_center, 1.f);

            l_Rec.x = 600;
        }
        else if (time < OUPI_GOUPI_ENTRANCE_DURATION + OUPI_GOUPI_PRE_PUNCH_DURATION + OUPI_GOUPI_PUNCH_DURATION + OUPI_GOUPI_EXPLOSION_PRE_DURATION)
        {
            l_Rec.x = 600;
        }
        else if (time < OUPI_GOUPI_ENTRANCE_DURATION + OUPI_GOUPI_PRE_PUNCH_DURATION + OUPI_GOUPI_PUNCH_DURATION + OUPI_GOUPI_EXPLOSION_PRE_DURATION + OUPI_GOUPI_EXPLOSION_DURATION)
        {
            if (l_LastTime < OUPI_GOUPI_ENTRANCE_DURATION + OUPI_GOUPI_PRE_PUNCH_DURATION + OUPI_GOUPI_PUNCH_DURATION + OUPI_GOUPI_EXPLOSION_PRE_DURATION)
            {
                // Play explosion sound
                AudioManager_play(g_gameConfig.audio, self->m_explosionAudio);
            }

            float fixedTime = time - (OUPI_GOUPI_ENTRANCE_DURATION + OUPI_GOUPI_PRE_PUNCH_DURATION + OUPI_GOUPI_PUNCH_DURATION + OUPI_GOUPI_EXPLOSION_PRE_DURATION);

            int index = (Float_clamp(fixedTime, 0, OUPI_GOUPI_EXPLOSION_DURATION) / OUPI_GOUPI_EXPLOSION_DURATION) * 3;
            //printf("%f\n", fixedTime);

            l_Rec.x = 600;
            l_Rec.w *= 2.f;
            l_Rec.h *= 2.f;
            SpriteGroup_render(self->m_explosionSprite, index, &l_Rec, Vec2_anchor_center, 1.f);
        }
        else if (time < OUPI_GOUPI_ENTRANCE_DURATION + OUPI_GOUPI_PRE_PUNCH_DURATION + OUPI_GOUPI_PUNCH_DURATION + OUPI_GOUPI_EXPLOSION_PRE_DURATION + OUPI_GOUPI_EXPLOSION_DURATION + OUPI_GOUPI_EXPLOSION_POST_DURATION)
        {
            g_gameConfig.state = PLAYING;
        }

        if (g_gameConfig.gamblingAnimTime < OUPI_GOUPI_ENTRANCE_DURATION + OUPI_GOUPI_PRE_PUNCH_DURATION + OUPI_GOUPI_PUNCH_DURATION + OUPI_GOUPI_EXPLOSION_PRE_DURATION)
        {
            SpriteGroup* l_Spr = self->m_oupiGoupiSprite;

            SpriteGroup_render(l_Spr, 0, &l_Rec, Vec2_anchor_center, 1.f);
        }
    }

    if (g_gameConfig.state == GAMBLING && g_gameConfig.gamblingResult == FISC_GUY)
    {
        float l_LastTime = g_gameConfig.gamblingAnimTime;

        g_gameConfig.gamblingAnimTime += Timer_getDelta(g_time);
        float l_FixedAnimTime = g_gameConfig.gamblingAnimTime;
        int l_Pos = 0;
        int l_LastPos = self->m_lastFiscPos;

        float l_TargetPosY = g_gameConfig.fiscTargetPos.y;

        float l_Div = g_gameConfig.fiscTargetPos.x / 600;
        float l_Div2 = g_gameConfig.fiscNextMovePos.x / 600;

        float l_Entrance = l_Div * FISC_ENTRANCE;

        if (l_FixedAnimTime < l_Entrance)
        {
            l_Pos = (l_FixedAnimTime) * (g_gameConfig.fiscTargetPos.x / l_Entrance);
            l_Pos -= l_Pos % 40;

            if (l_LastPos != l_Pos)
            {
                self->m_fiscGuyScale *= -1;
                AudioManager_play(g_gameConfig.audio, self->m_fiscGuyScale == -1 ? g_gameConfig.footstep0 : g_gameConfig.footstep1);
            }
            l_LastPos = l_Pos;
        }
        else if (l_FixedAnimTime < l_Entrance + FISC_PRE_MOVE_DUR)
        {
            l_Pos = g_gameConfig.fiscTargetPos.x;
            l_LastPos = l_Pos;
        } else if (l_FixedAnimTime < l_Entrance + FISC_PRE_MOVE_DUR + FISC_POST_MOVE_DUR)
        {
            if (l_Pos != l_LastPos)
            {
                g_gameConfig.core->Rabbits[g_gameConfig.solveObjectIndex] = g_gameConfig.solveNextSol[1];
            }

            l_Pos = g_gameConfig.fiscNextMovePos.x;
            l_TargetPosY = g_gameConfig.fiscNextMovePos.y;
            l_LastPos = l_Pos;
        }
        else if (l_FixedAnimTime < l_Entrance + FISC_PRE_MOVE_DUR + FISC_POST_MOVE_DUR + (FISC_EXIT * l_Div2))
        {

            float l_AnimTime = l_FixedAnimTime - l_Entrance - FISC_PRE_MOVE_DUR - FISC_POST_MOVE_DUR;
            l_Pos = g_gameConfig.fiscNextMovePos.x - ((l_AnimTime) * (g_gameConfig.fiscNextMovePos.x / (FISC_EXIT * l_Div2)));
            l_Pos -= l_Pos % 40;

            if (l_LastPos != l_Pos)
            {
                self->m_fiscGuyScale *= -1;
                AudioManager_play(g_gameConfig.audio, self->m_fiscGuyScale == -1 ? g_gameConfig.footstep0 : g_gameConfig.footstep1);
            }
            l_TargetPosY = g_gameConfig.fiscNextMovePos.y;
            l_LastPos = l_Pos;
        }
        else if (l_FixedAnimTime >= l_Entrance + FISC_PRE_MOVE_DUR + FISC_POST_MOVE_DUR + (FISC_EXIT * l_Div2))
        {
            g_gameConfig.state = PLAYING;
            g_gameConfig.gamblingResult = NONE;
            self->m_lastFiscPos = -1;
        }

        self->m_lastFiscPos = l_LastPos;

        SDL_FRect l_CatRect = { 0 };
        l_CatRect.x = l_Pos;
        l_CatRect.y = l_TargetPosY;
        l_CatRect.w = self->m_fiscGuyScale * 40.f;
        l_CatRect.h = 42.f;

        SpriteGroup_render(self->m_lowQualityCatSprite, 0, &l_CatRect, Vec2_anchor_center, 1.f);
    }
}

float ModFloat(float v, float x)
{
    {
        float val = v;
        if (val < 0)
        {
            while (val < x)
                val += x;
        }
        else
        {
            while (val > x)
                val -= x;
        }

        return val;
    }

}
