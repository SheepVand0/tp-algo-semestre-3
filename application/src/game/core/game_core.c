/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "game_core.h"
#include "game/scene.h"

#define RABBIT(x) &gameCore->Rabbits[x]

GameCore* GameCore_create()
{
    GameCore* l_Core = malloc(sizeof(GameCore));
    assert(l_Core);

    l_Core->Selected = NULL;
    l_Core->State = NONE;
    l_Core->Remaining = 10.f;

    return l_Core;
}

void GameCore_update(GameCore* gameCore, Scene* scene, int selectedX, int selectedY)
{

    switch (gameCore->State)
    {
    case NONE:
        GameCore_initNextGame(gameCore);
        if (!g_gameConfig.isEditing)
            gameCore->State = PLAYING;
        else
        {
            gameCore->State = EDITING;
        }
        break;

    case PLAYING:
        for (int x = 0; x < RABBIT_COUNT + FOX_COUNT + MUSHROOM_COUNT; x++)
        {
            Rabbit* l_Rabb = RABBIT(x);
            if (l_Rabb)
            {
                if (!l_Rabb->Movable) continue;

                bool l_Other = false;
                if (l_Rabb->Type == FOX)
                {
                    Vec2 l_OtherCell = Fox_getSecondCell(l_Rabb);
                    l_Other = l_OtherCell.x == selectedX && l_OtherCell.y == selectedY;

                    //printf("%f - %d | %f - %d\n", l_OtherCell.x, self->m_gameGraphics->m_selectedColIndex, l_OtherCell.y, self->m_gameGraphics->m_selectedRowIndex);
                }

                if ((l_Rabb->CellX == selectedX && l_Rabb->CellY == selectedY) || l_Other)
                {
                    gameCore->Selected = l_Rabb;
                }
            }
        }

        if (scene->m_input->mouse.leftPressed)
        {
            EObjectType l_Obj = GameCore_getObjTypeAtLocation(gameCore, selectedX, selectedY);

            if (l_Obj == NO_OBJECT)
            {
                if (gameCore->Selected)
                {
                    Rabbit* l_Rabb = gameCore->Selected;

                    Rabbit_move(l_Rabb, gameCore, selectedX, selectedY);
                }
            }
        }

        gameCore->Remaining -= Timer_getDelta(g_time);

        if (gameCore->Remaining <= 0)
        {
            AudioManager_play(g_gameConfig.Audio, g_gameConfig.LarryAudio);
            gameCore->State = GETTING_LARRIED;
            gameCore->CurrentAnimationTime = 6.f;
        }
        break;
    case WINNING:

        break;
    case GETTING_LARRIED:
        gameCore->CurrentAnimationTime -= Timer_getDelta(g_time);

        if (gameCore->CurrentAnimationTime <= 0)
        {
            gameCore->State = NONE;
            g_gameConfig.nextScene = GAME_SCENE_MAIN;
            g_gameConfig.inLevel = false;
            scene->m_uiManager->m_nextAction = GAME_UI_ACTION_OPEN_TITLE;
        }
        break;
    default: break;
    }
    
}

void GameCore_destroyGame(GameCore* gameCore)
{
    for (int x = 0; x < RABBIT_COUNT + FOX_COUNT + MUSHROOM_COUNT; x++)
    {
        Rabbit l_Zero = { 0 };
        gameCore->Rabbits[x] = l_Zero;
    }
}

void GameCore_initNextGame(GameCore* gameCore)
{
    if (g_gameConfig.isEditing)
    {
        g_gameConfig.Settings->FoxCount = 0;
        g_gameConfig.Settings->MushroomCount = 0;
        g_gameConfig.Settings->RabbitCount = 0;

        return;
    }

    for (int x = 0; x < RABBIT_COUNT; x++)
    {
        gameCore->Rabbits[x] = *Rabbit_create(gameCore, x + 2, GAME_GRID_SIZE / 2);
    }
    for (int x = RABBIT_COUNT; x < RABBIT_COUNT + FOX_COUNT; x++)
    {
        gameCore->Rabbits[x] = *Fox_create(gameCore, x + 2, GAME_GRID_SIZE / 2, RABBIT_EAST);
    }
    for (int x = RABBIT_COUNT + FOX_COUNT; x < RABBIT_COUNT + FOX_COUNT + MUSHROOM_COUNT; x++)
    {
        gameCore->Rabbits[x] = *Mushroom_create(gameCore, x - 4, GAME_GRID_SIZE / 2 + 1);
    }
}


EObjectType GameCore_getObjTypeAtLocation(GameCore* gameCore, int cellx, int y)
{

    for (int x = 0; x < RABBIT_COUNT + FOX_COUNT + MUSHROOM_COUNT; x++)
    {
        Rabbit* l_Rabb = &gameCore->Rabbits[x];

        bool l_Other = false;
        if (l_Rabb->Type == FOX)
        {
            Vec2 l_OtherCell = Fox_getSecondCell(l_Rabb);
            l_Other = l_OtherCell.x == cellx && l_OtherCell.y == y;
        }

        if ((l_Rabb->CellX == cellx && l_Rabb->CellY == y) || l_Other)
        {
            return l_Rabb->Type;
        }

    }

    return NO_OBJECT;
}

Rabbit* GameCore_getRabbit(GameCore* gameCore, int index)
{
    return &(gameCore->Rabbits[index]);
}

Rabbit* GameCore_getFox(GameCore* gameCore, int index)
{
    return &(gameCore->Rabbits[index + g_gameConfig.Settings->RabbitCount]);
}

Rabbit* GameCore_getMushroom(GameCore* gameCore, int index)
{
    return &(gameCore->Rabbits[index + g_gameConfig.Settings->RabbitCount + g_gameConfig.Settings->FoxCount]);
}

Rabbit* Rabbit_create(GameCore* gameCore, int cellX, int cellY)
{
    Rabbit* l_Rabbit = malloc(sizeof(Rabbit));

    assert(l_Rabbit);
    l_Rabbit->CellX = cellX;
    l_Rabbit->CellY = cellY;

    AssetManager* assets = g_gameConfig.Assets;
    SpriteSheet* spriteSheet = AssetManager_getSpriteSheet(assets, SPRITE_GAME);
    AssertNew(spriteSheet);
    l_Rabbit->RabbitSprite = SpriteSheet_getGroupByName(spriteSheet, "rabbit");
    AssertNew(l_Rabbit->RabbitSprite);

    l_Rabbit->Movable = true;
    l_Rabbit->Type = RABBIT;

    return l_Rabbit;
}

void Rabbit_destroy(Rabbit* rabbit)
{
    free(rabbit);
}

int signOf(int x)
{
    if (x == 0) return 0;

    return x < 0 ? -1 : 1;
}

bool Rabbit_canMove(Rabbit* rabbit, GameCore* gameCore, int targetX, int targetY)
{
    int indexX = rabbit->CellX;
    int indexY = rabbit->CellY;

    if (indexX != targetX && indexY != targetY) return false;

    if (indexX == targetX && targetY == indexY) return false;

    if (targetX < 0 || targetX > GAME_GRID_SIZE || targetY < 0 || targetY > GAME_GRID_SIZE) return false;

    int directionX = signOf(rabbit->CellX - targetX);
    int directionY = signOf(rabbit->CellY - targetY);

    int l_ToSubX = rabbit->CellX - targetX;
    int l_ToSubY = rabbit->CellY - targetY;

    Vec2 l_Dir = Fox_getDirection(rabbit);
    if (rabbit->Type == FOX)
    {
        if (directionX == -l_Dir.x && directionY == -l_Dir.y)
        {
            indexX -= directionX;
            indexY -= directionY;

            l_ToSubX -= directionX;
            l_ToSubY -= directionY;

        }

        if (abs(directionX) != abs(l_Dir.x) && abs(directionY) != abs(l_Dir.y))
        {
            printf("dir not valid\n");
            return false;
        }
    }

    int itCount = 0;

    Vec2 l_SecondCell = Fox_getSecondCell(rabbit);

    do
    {
        indexX -= directionX;
        indexY -= directionY;

        EObjectType l_Type = GameCore_getObjTypeAtLocation(gameCore, indexX, indexY);

        if (rabbit->Type == FOX)
        {
            if (l_Type != NO_OBJECT)
            {
                return false;
            }

            if (l_Type == NO_OBJECT && indexX == targetX && indexY == targetY)
            {
                return true;
            }

            continue;
        }

        if ((l_Type == NO_OBJECT && itCount == 0) || rabbit->Type != RABBIT) return false;

        itCount++;

        if (l_Type == NO_OBJECT)
        {
            if ((targetX != indexX || targetY != indexY))
            {
                return false;
            }

            return true;
        }
    } while (true);

    return false;
}

bool Rabbit_move(Rabbit* rabbit, GameCore* gameCore, int targetX, int targetY)
{
    int indexX = rabbit->CellX;
    int indexY = rabbit->CellY;

    if (indexX != targetX && indexY != targetY) return false;

    if (indexX == targetX && targetY == indexY) return false;

    if (targetX < 0 || targetX > GAME_GRID_SIZE || targetY < 0 || targetY > GAME_GRID_SIZE) return false;

    int directionX = signOf(rabbit->CellX - targetX);
    int directionY = signOf(rabbit->CellY - targetY);

    int l_ToSubX = rabbit->CellX - targetX;
    int l_ToSubY = rabbit->CellY - targetY;

    Vec2 l_Dir = Fox_getDirection(rabbit);
    if (rabbit->Type == FOX)
    {
        if (directionX == -l_Dir.x && directionY == -l_Dir.y)
        {
            indexX -= directionX;
            indexY -= directionY;

            l_ToSubX -= directionX;
            l_ToSubY -= directionY;

        }

        /*if (abs(targetX - indexX) > 1 || abs(targetY - indexY) > 1)
        {
            printf("Distance to high\n");
            return false;
        }*/

        if (abs(directionX) != abs(l_Dir.x) && abs(directionY) != abs(l_Dir.y))
        {
            printf("dir not valid\n");
            return false;
        }
    }

    int itCount = 0;

    Vec2 l_SecondCell = Fox_getSecondCell(rabbit);

    do
    {
        indexX -= directionX;
        indexY -= directionY;

        EObjectType l_Type = GameCore_getObjTypeAtLocation(gameCore, indexX, indexY);

        if (rabbit->Type == FOX)
        {
            if (l_Type != NO_OBJECT)
            {
                return false;
            }

            if (l_Type == NO_OBJECT && indexX == targetX && indexY == targetY)
            {
                rabbit->CellX -= l_ToSubX;
                rabbit->CellY -= l_ToSubY;
                return true;
            }

            continue;
        }

        if ((l_Type == NO_OBJECT && itCount == 0) || rabbit->Type != RABBIT) return false;

        itCount++;

        if (l_Type == NO_OBJECT)
        {
            if ((targetX != indexX || targetY != indexY))
            {
                return false;
            }

            rabbit->CellX = indexX;
            rabbit->CellY = indexY;
            return true;
        }
    } while (true);

    return false;
}

Rabbit* Fox_create(GameCore* gameCore, int cellX0, int cellY0, ERabbitDirection direction)
{
    Rabbit* l_Fox = malloc(sizeof(Rabbit));

    assert(l_Fox);
    l_Fox->CellX = cellX0;
    l_Fox->CellY = cellY0;
    l_Fox->Direction = direction;

    AssetManager* assets = g_gameConfig.Assets;
    SpriteSheet* spriteSheet = AssetManager_getSpriteSheet(assets, SPRITE_GAME);
    AssertNew(spriteSheet);
    l_Fox->RabbitSprite = SpriteSheet_getGroupByName(spriteSheet, "fox");
    AssertNew(l_Fox->RabbitSprite);

    l_Fox->Type = FOX;
    l_Fox->Movable = true;

    return l_Fox;
}

Vec2 Fox_getSecondCell(Rabbit* rabbit)
{
    switch (rabbit->Direction)
    {
    case RABBIT_NORTH:
        return Vec2_set(rabbit->CellX, rabbit->CellY - 1);
    case RABBIT_EAST:
        return Vec2_set(rabbit->CellX + 1, rabbit->CellY);
    case RABBIT_WEST:
        return Vec2_set(rabbit->CellX - 1, rabbit->CellY);
    case RABBIT_SOUTH:
        return Vec2_set(rabbit->CellX, rabbit->CellY + 1);
    default: return Vec2_set(rabbit->CellX, rabbit->CellY);
    }
}

Vec2 Fox_getDirection(Rabbit* rabbit)
{
    switch (rabbit->Direction)
    {
    case RABBIT_NORTH:
        return Vec2_set(0, -1);
    case RABBIT_EAST:
        return Vec2_set(1, 0);
    case RABBIT_WEST:
        return Vec2_set(-1, 0);
    case RABBIT_SOUTH:
        return Vec2_set(0, 1);
    default: return Vec2_set(0, 0);
    }
}

void Rabbit_getAnchorAngAngleFromDirection(ERabbitDirection direction, Vec2* anchor, float* angle)
{
    float l_Angle = 0.f;
    Vec2 l_Anchor = Vec2_anchor_north_west;

    switch (direction)
    {
    case RABBIT_NORTH:
        l_Angle = -180.f;
        l_Anchor = Vec2_set(1.f, .5f);
        break;
    case RABBIT_EAST:
        l_Angle = -90.f;
        l_Anchor = Vec2_set(1.f, 1.f);
        break;
    case RABBIT_WEST:
        l_Angle = 90.f;
        l_Anchor = Vec2_set(0.f, .5f);
        break;
    case RABBIT_SOUTH:
        l_Angle = 0.f;
        break;
    default: break;

    }

    *anchor = l_Anchor;
    *angle = l_Angle;
}

Rabbit* Mushroom_create(GameCore* gameCore, int cellX, int cellY)
{
    Rabbit* l_Mush = malloc(sizeof(Rabbit));
    assert(l_Mush);

    l_Mush->CellX = cellX;
    l_Mush->CellY = cellY;

    AssetManager* assets = g_gameConfig.Assets;
    SpriteSheet* spriteSheet = AssetManager_getSpriteSheet(assets, SPRITE_GAME);
    AssertNew(spriteSheet);
    l_Mush->RabbitSprite = SpriteSheet_getGroupByName(spriteSheet, "mushroom");
    AssertNew(l_Mush->RabbitSprite);

    l_Mush->Type = MUSHROOM;
    l_Mush->Movable = false;

    return l_Mush;
}
