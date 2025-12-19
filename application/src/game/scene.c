/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "game/scene.h"
#include "game/game_config.h"
#include "core/game_core.h"
#include "game_editor.h"
#include "game/game_settings.h"

#define RABBIT_ARRAY g_gameConfig.core->Rabbits
#define RABBIT(x) &g_gameConfig.core->Rabbits[x]

#define GAMBLING_PRE_ANIM_DURATION 5.f

Scene* Scene_create()
{
    Scene* self = (Scene*)calloc(1, sizeof(Scene));
    AssertNew(self);

    self->m_assets = AssetManager_create(SPRITE_COUNT, FONT_COUNT);
    Game_addAssets(self->m_assets);


    self->m_input = Input_create();

    int outW = 0, outH = 0;
    SDL_GetRenderOutputSize(g_renderer, &outW, &outH);

    self->m_camera = Camera_create(outW, outH);
    self->m_state = SCENE_STATE_FADING_IN;
    self->m_fadingTime = 0.5f;
    self->m_uiManager = GameUIManager_create(self);
    self->m_gameGraphics = GameGraphics_create(self);

    g_gameConfig.core = GameCore_create();
    g_gameConfig.assets = self->m_assets;
    //self->Obstacles = NULL;

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
    if (g_gameConfig.inputLockTime <= 0)
        Input_update(self->m_input);

    if (g_gameConfig.inputLockTime >= 0)
        g_gameConfig.inputLockTime -= Timer_getDelta(g_time);

    Camera_updateViewport(self->m_camera, g_renderer);
    AssetManager_updateFontSizes(self->m_assets);

    GameUIManager_update(self->m_uiManager, &(self->m_input->uiInput));

    if (g_gameConfig.inLevel)
    {
        GameGraphics_update(self->m_gameGraphics);

        GameCore_update(g_gameConfig.core, self, self->m_gameGraphics->m_selectedColIndex, self->m_gameGraphics->m_selectedRowIndex);
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

    /// 1150 - 175

    SDL_Color bgColor = { 0 };
    bgColor.r = 15;
    bgColor.g = 15;
    bgColor.b = 15;
    bgColor.a = 255;
    SDL_SetRenderDrawColor(g_renderer, bgColor.r, bgColor.g, bgColor.b, 255);
    SDL_RenderFillRect(g_renderer, NULL);

    GameUIManager_render(self->m_uiManager);

    if (g_gameConfig.inLevel && g_gameConfig.state != NONE && g_gameConfig.state != GETTING_LARRIED && g_gameConfig.state != WINNING)
    {
        GameCore* gameCore = g_gameConfig.core;

        GAME_GRAPHICS_RENDER(self->m_gameGraphics,
            g_gameConfig.isEditing ? GameEditor_buildUsableArray(g_gameConfig.core->Rabbits) : g_gameConfig.core->Rabbits,
            RABBIT_COUNT + FOX_COUNT + MUSHROOM_COUNT);
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

    if (g_gameConfig.state == GETTING_LARRIED || g_gameConfig.state == WINNING)
    {
        SpriteGroup* l_Larry = NULL;
        
        SDL_FRect l_Rec;

        l_Rec.w = 1280;
        l_Rec.h = 720;
        l_Rec.x = l_Rec.w / 2.f;
        l_Rec.y = l_Rec.h / 2;

        

        if (g_gameConfig.state == GETTING_LARRIED)
        {
            l_Larry = SpriteSheet_getGroupByName(AssetManager_getSpriteSheet(Scene_getAssetManager(self), SPRITE_LARRY), "larry");
            SpriteGroup_setColorModFloat(l_Larry, 1.f, 0.5f, 0.5f);
        }
        else
        {
            l_Larry = SpriteSheet_getGroupByName(AssetManager_getSpriteSheet(Scene_getAssetManager(self), SPRITE_ELECTRICIAN), "electrician");
            SpriteGroup_setColorModFloat(l_Larry, 0.5f, 0.5f, 1.f);
            l_Rec.w /= 1.5f;
        }
        
        SpriteGroup_renderRotated(l_Larry, 0, &l_Rec, Vec2_set(0.5f, 0.5f), /*Timer_getElapsed(g_time) * 360*/ 0, 0);

        GameUIManager_render(self->m_uiManager);

        SDL_SetRenderDrawColor(g_renderer, 0, 0, 0, 255 - Float_clamp((sin(g_gameConfig.currentAnimationTime / 6.f * M_PI) * (255.f)), 0, 255));
        SDL_RenderFillRect(g_renderer, NULL);
    }
}

void Scene_drawGizmos(Scene* self)
{
    assert(self && "The Scene must be created");
}
