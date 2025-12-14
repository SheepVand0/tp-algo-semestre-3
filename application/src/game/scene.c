/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "game/scene.h"
#include "game/game_config.h"
#include "core/game_core.h"
#include "game/game_settings.h"

Scene* Scene_create()
{
    Scene* self = (Scene*)calloc(1, sizeof(Scene));
    AssertNew(self);

    self->m_assets = AssetManager_create(SPRITE_COUNT, FONT_COUNT);
    Game_addAssets(self->m_assets);

    self->m_gameSettings = GameSettings_create();

    self->m_input = Input_create();

    int outW = 0, outH = 0;
    SDL_GetRenderOutputSize(g_renderer, &outW, &outH);

    self->m_camera = Camera_create(outW, outH);
    self->m_state = SCENE_STATE_FADING_IN;
    self->m_fadingTime = 0.5f;
    self->m_uiManager = GameUIManager_create(self);
    self->m_gameGraphics = GameGraphics_create(self);

    self->Rabbits = NULL;
    self->Obstacles = NULL;

    g_gameConfig.nextScene = GAME_SCENE_QUIT;

    return self;
}

void Scene_destroy(Scene* self)
{
    if (!self) return;

    AssetManager_destroy(self->m_assets);
    Input_destroy(self->m_input);
    Camera_destroy(self->m_camera);
    GameUIManager_destroy(self->m_uiManager);
    GameGraphics_destroy(self->m_gameGraphics);

    // TODO : destroy rabbits and foxes and mushrooms
    // flm de le faire mtn

    GameSettings_destroy(self->m_gameSettings);

    free(self);
}

void Scene_mainLoop(Scene* self)
{
    assert(self && "The Scene must be created");
    Timer_setTimeScale(g_time, 1.f);
    while (true)
    {
        // Met à jour le temps
        Timer_update(g_time);

        Scene_update(self);

        Input* input = Scene_getInput(self);
        if (input->quitPressed)
        {
            g_gameConfig.nextScene = GAME_SCENE_QUIT;
            return;
        }

        if (self->m_state == SCENE_STATE_FINISHED)
            return;

        // Rend la scène
        Scene_render(self);

        // Affiche le nouveau rendu
        SDL_RenderPresent(g_renderer);
    }
}

void Scene_update(Scene* self)
{
    assert(self && "The Scene must be created");
    Game_updateSizes();
    Input_update(self->m_input);

    Camera_updateViewport(self->m_camera, g_renderer);
    AssetManager_updateFontSizes(self->m_assets);

    GameUIManager_update(self->m_uiManager, &(self->m_input->uiInput));

    if (g_gameConfig.inLevel)
    {
        GameGraphics_update(self->m_gameGraphics);

        for (int x = 0; x < self->m_gameSettings->RabbitCount; x++)
        {
            Rabbit* l_Rabb = self->Rabbits[x];
            if (l_Rabb)
            {
                if (l_Rabb->CellX == self->m_gameGraphics->m_selectedColIndex && l_Rabb->CellY == self->m_gameGraphics->m_selectedRowIndex)
                {
                    self->m_gameGraphics->Selected = SelectedObject_create((void*)l_Rabb, RABBIT);
                }
            }
        }

        if (self->m_input->mouse.leftPressed)
        {
            EObjectType l_Obj = Scene_getObjTypeAtLocation(self, self->m_gameGraphics->m_selectedColIndex, self->m_gameGraphics->m_selectedRowIndex);

            if (l_Obj == NO_OBJECT)
            {
                if (self->m_gameGraphics->Selected)
                {
                    if (self->m_gameGraphics->Selected->ObjectType == RABBIT)
                    {
                        Rabbit* l_Rabb = self->m_gameGraphics->Selected->SelectedRabbit;

                        Rabbit_move(l_Rabb, self, self->m_gameGraphics->m_selectedColIndex, self->m_gameGraphics->m_selectedRowIndex);
                    }
                }
            }
        }
    }

    if (self->m_input->debug.gizmosPressed)
    {
        g_drawUIGizmos = !g_drawUIGizmos;
    }

    if (self->m_state == SCENE_STATE_FADING_IN)
    {
        self->m_accu += Timer_getUnscaledDelta(g_time);
        if (self->m_accu >= self->m_fadingTime)
        {
            self->m_state = SCENE_STATE_RUNNING;
        }
    }
    if (self->m_state == SCENE_STATE_FADING_OUT)
    {
        self->m_accu += Timer_getUnscaledDelta(g_time);
        if (self->m_accu >= self->m_fadingTime)
        {
            self->m_state = SCENE_STATE_FINISHED;
        }
    }
}

void Scene_quit(Scene* self)
{
    assert(self && "The Scene must be created");
    if ((self->m_state == SCENE_STATE_FADING_OUT) ||
        (self->m_state == SCENE_STATE_FINISHED))
        return;

    self->m_state = SCENE_STATE_FADING_OUT;
    self->m_accu = 0.f;
}

void Scene_render(Scene* self)
{
    assert(self && "The Scene must be created");

    // Efface le rendu précédent
    SDL_SetRenderDrawColor(g_renderer, 0, 0, 0, 255);
    SDL_RenderClear(g_renderer);

    SDL_Color bgColor = { 0 };
    bgColor.r = 15;
    bgColor.g = 15;
    bgColor.b = 15;
    bgColor.a = 255;
    SDL_SetRenderDrawColor(g_renderer, bgColor.r, bgColor.g, bgColor.b, 255);
    SDL_RenderFillRect(g_renderer, NULL);

    GameUIManager_render(self->m_uiManager);

    if (g_gameConfig.inLevel)
    {

        GAME_GRAPHICS_RENDER(self->m_gameGraphics, self->Rabbits, self->m_gameSettings->RabbitCount, self->Obstacles, self->m_gameSettings->MushroomCount + self->m_gameSettings->RabbitCount);
        GameGraphics_render(self->m_gameGraphics);
    }

    // Fading
    if ((self->m_state == SCENE_STATE_FADING_IN) ||
        (self->m_state == SCENE_STATE_FADING_OUT))
    {
        int opacity = (int)(255.f * self->m_accu / self->m_fadingTime);
        if (self->m_state == SCENE_STATE_FADING_IN)
        {
            opacity = 255 - opacity;
        }
        SDL_SetRenderDrawColor(g_renderer, 0, 0, 0, opacity);
        SDL_RenderFillRect(g_renderer, NULL);
    }
}

void Scene_drawGizmos(Scene* self)
{
    assert(self && "The Scene must be created");
}

void Scene_destroyGame(Scene* self)
{
    if (self->Rabbits)
    {
        for (int x = 0; x < self->m_gameSettings->RabbitCount; x++)
        {
            Rabbit_destroy(self->Rabbits[x]);
        }
    }

    if (self->Obstacles)
    {
        for (int x = 0; x < self->m_gameSettings->MushroomCount + self->m_gameSettings->FoxCount; x++)
        {
            Obstacle_destroy(self->Obstacles[x]);
        }
    }
}

void Scene_initGame(Scene* self)
{
    Scene_destroyGame(self);
        
    self->Rabbits = calloc(self->m_gameSettings->RabbitCount, sizeof(Obstacle));
    assert(self->Rabbits);
    for (int x = 0; x < self->m_gameSettings->RabbitCount; x++)
    {
        self->Rabbits[x] = Rabbit_create(self, x, GAME_GRID_SIZE / 2);
    }
    self->Obstacles = calloc(self->m_gameSettings->MushroomCount + self->m_gameSettings->FoxCount, sizeof(Obstacle));
    assert(self->Obstacles);
    for (int x = 0; x < self->m_gameSettings->MushroomCount + self->m_gameSettings->FoxCount; x++)
    {
        self->Obstacles[x] = Fox_create(self, 0, 0, 0, 0);
    }
}

EObjectType Scene_getObjTypeAtLocation(Scene* self, int cellx, int y)
{

    for (int x = 0; x < RABBIT_COUNT; x++)
    {
        if (self->Rabbits[x])
        {
            if (self->Rabbits[x]->CellX == cellx && self->Rabbits[x]->CellY == y)
            {
                return RABBIT;
            }
        }
    }

    for (int x = 0; x < OBSTACLE_COUNT; x++)
    {
        if (self->Obstacles[x])
        {
            AABB l_AABB = { 0 };
            l_AABB.lower = Vec2_set(self->Obstacles[x]->CellX0, self->Obstacles[x]->CellY0);
            l_AABB.upper = Vec2_set(self->Obstacles[x]->CellX1, self->Obstacles[x]->CellY1);

            if (AABB_containsPoint(&l_AABB, Vec2_set(cellx, y)))
            {
                return OBSTACLE;
            }
        }
    }

    return NO_OBJECT;
}
