/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "game_core.h"
#include "game/scene.h"
#include "game/game_loader.h"

#define RABBIT(x) &gameCore->Rabbits[x]

GameCore* GameCore_create()
{
    GameCore* l_Core = malloc(sizeof(GameCore));
    assert(l_Core);

    return l_Core;
}

void GameCore_update(GameCore* gameCore, Scene* scene, int selectedX, int selectedY)
{

    switch (g_gameConfig.State)
    {
    case NONE:
        if (!g_gameConfig.isEditing)
        {
            //GameCore_initNextGame(gameCore);
            g_gameConfig.State = PLAYING;
        }
        else
        {
            g_gameConfig.State = EDITING;
        }
        break;

    case PLAYING:
        int validRabbCount = 0;
        for (int x = 0; x < MAX_RABBITS + MAX_FOXES + MAX_MUSHROOMS; x++)
        {
            Rabbit* l_Rabb = RABBIT(x);
            if (l_Rabb)
            {
                if (l_Rabb->Type == RABBIT)
                {
                    int i = l_Rabb->CellX;
                    int j = l_Rabb->CellY;

                    validRabbCount += (i == GAME_GRID_SIZE / 2 && j == GAME_GRID_SIZE / 2 || i == 0 && j == 0 || j == 0 && i == GAME_GRID_SIZE - 1 || i == 0 && j == GAME_GRID_SIZE - 1 || (i == GAME_GRID_SIZE - 1 && j == GAME_GRID_SIZE - 1));
                }

                if (l_Rabb->Type != NONE)
                {
                    if (!l_Rabb->Movable) continue;

                    bool l_Other = false;
                    if (l_Rabb->Type == FOX)
                    {
                        Vec2 l_OtherCell = Fox_getSecondCell(l_Rabb);
                        l_Other = l_OtherCell.x == selectedX && l_OtherCell.y == selectedY;
                    }

                    if ((l_Rabb->CellX == selectedX && l_Rabb->CellY == selectedY) || l_Other)
                    {
                        g_gameConfig.Selected = l_Rabb;
                    }
                }
            }
        }

        if (validRabbCount == RABBIT_COUNT && !g_gameConfig.isEditing)
        {
            g_gameConfig.State = WINNING;
            g_gameConfig.CurrentAnimationTime = 6.f;
            AudioManager_play(g_gameConfig.Audio, g_gameConfig.AmongUsAudio);

            UILabel_setTextString(scene->m_uiManager->m_lostText, "You won!");
            UILabel_setColor(scene->m_uiManager->m_lostText, g_colors.green6);
        }

        if (scene->m_input->mouse.leftPressed)
        {
            EObjectType l_Obj = GameCore_getObjTypeAtLocation(gameCore, selectedX, selectedY);

            if (l_Obj == NO_OBJECT)
            {
                if (g_gameConfig.Selected)
                {
                    if (g_gameConfig.Selected->CellX != selectedX || g_gameConfig.Selected->CellY != selectedY)
                    {

                        Rabbit* l_Rabb = g_gameConfig.Selected;

                        Rabbit_move(l_Rabb, gameCore, selectedX, selectedY);
                    }
                }
            }
        }

        if (g_gameConfig.isEditing == false)
        {
            g_gameConfig.Remaining -= Timer_getDelta(g_time);

            if (g_gameConfig.Remaining <= 0)
            {
                AudioManager_play(g_gameConfig.Audio, g_gameConfig.LarryAudio);
                g_gameConfig.State = GETTING_LARRIED;
                g_gameConfig.CurrentAnimationTime = 6.f;

                UILabel_setTextString(scene->m_uiManager->m_lostText, "You lost");
                UILabel_setColor(scene->m_uiManager->m_lostText, g_colors.red6);
            }
        }
        break;
    case WINNING:
    case GETTING_LARRIED:
        g_gameConfig.CurrentAnimationTime -= Timer_getDelta(g_time);

        if (g_gameConfig.CurrentAnimationTime <= 0)
        {
            g_gameConfig.State = NONE;
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
    for (int x = 0; x < MAX_RABBITS + MAX_FOXES + MAX_MUSHROOMS; x++)
    {
        Rabbit l_Zero = { 0 };
        gameCore->Rabbits[x] = l_Zero;
    }

    g_gameConfig.State = NONE;
}

void GameCore_initNextGame(GameCore* gameCore)
{
    if (g_gameConfig.isEditing)
    {
        for (int x = 0; x < RABBIT_COUNT; x++)
        {
            gameCore->Rabbits[x] = *Rabbit_create(gameCore, 0, 0);
        }
        for (int x = MAX_RABBITS; x < MAX_RABBITS + FOX_COUNT; x++)
        {
            gameCore->Rabbits[x] = *Fox_create(gameCore, 0, 0, RABBIT_EAST);
        }
        for (int x = MAX_RABBITS + MAX_FOXES; x < MAX_RABBITS + MAX_FOXES + MUSHROOM_COUNT; x++)
        {
            gameCore->Rabbits[x] = *Mushroom_create(gameCore, 0, 0);
        }
    }
    else
    {
        g_gameConfig.Remaining = g_gameConfig.Settings->TotalTime = 10.f;

        GameCore_destroyGame(g_gameConfig.Core);

        g_gameConfig.Settings->RabbitCount = 10;
        g_gameConfig.Settings->RabbitCount = 1;
        g_gameConfig.Settings->RabbitCount = 4;

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
}


EObjectType GameCore_getObjTypeAtLocation(GameCore* gameCore, int cellx, int y)
{

    for (int x = 0; x < MAX_RABBITS + MAX_FOXES + MAX_MUSHROOMS; x++)
    {
        Rabbit* l_Rabb = &gameCore->Rabbits[x];

        if (l_Rabb->Type == NONE) continue;

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
    l_Rabbit->Direction = RABBIT_NORTH;

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

    //printf("%d %d %d %d\n", indexX, indexY, targetX, targetY);

    if (indexX != targetX && indexY != targetY) return false;

    if (indexX == targetX && indexY == targetY) return false;

    if (indexX == targetX && targetY == indexY) return false;

    if (targetX < 0 || targetX > GAME_GRID_SIZE || targetY < 0 || targetY > GAME_GRID_SIZE) return false;

    int directionX = signOf(rabbit->CellX - targetX);
    int directionY = signOf(rabbit->CellY - targetY);

    int l_ToSubX = rabbit->CellX - targetX;
    int l_ToSubY = rabbit->CellY - targetY;

    Vec2 l_Dir = Fox_getDirection(rabbit);
    Vec2 l_Second = Fox_getSecondCell(rabbit);
    if (rabbit->Type == FOX)
    {
        if (indexX == l_Second.x && indexY == l_Second.y) return false;

        if (directionX == -l_Dir.x && directionY == -l_Dir.y)
        {
            indexX -= directionX;
            indexY -= directionY;

            l_ToSubX -= directionX;
            l_ToSubY -= directionY;
        }

        if (abs(directionX) != abs(l_Dir.x) && abs(directionY) != abs(l_Dir.y))
        {
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

    //printf("%d %d %d %d\n", indexX, indexY, targetX, targetY);

    if (indexX != targetX && indexY != targetY) return false;

    if (indexX == targetX && indexY == targetY) return false;

    if (indexX == targetX && targetY == indexY) return false;

    if (targetX < 0 || targetX > GAME_GRID_SIZE || targetY < 0 || targetY > GAME_GRID_SIZE) return false;

    int directionX = signOf(rabbit->CellX - targetX);
    int directionY = signOf(rabbit->CellY - targetY);

    int l_ToSubX = rabbit->CellX - targetX;
    int l_ToSubY = rabbit->CellY - targetY;

    Vec2 l_Dir = Fox_getDirection(rabbit);
    Vec2 l_Second = Fox_getSecondCell(rabbit);
    if (rabbit->Type == FOX)
    {
        if (indexX == l_Second.x && indexY == l_Second.y) return false;

        if (directionX == -l_Dir.x && directionY == -l_Dir.y)
        {
            indexX -= directionX;
            indexY -= directionY;

            l_ToSubX -= directionX;
            l_ToSubY -= directionY;
        }

        if (abs(directionX) != abs(l_Dir.x) && abs(directionY) != abs(l_Dir.y))
        {
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
    l_Mush->Direction = RABBIT_NORTH;

    AssetManager* assets = g_gameConfig.Assets;
    SpriteSheet* spriteSheet = AssetManager_getSpriteSheet(assets, SPRITE_GAME);
    AssertNew(spriteSheet);
    l_Mush->RabbitSprite = SpriteSheet_getGroupByName(spriteSheet, "mushroom");
    AssertNew(l_Mush->RabbitSprite);


    l_Mush->Type = MUSHROOM;
    l_Mush->Movable = false;

    return l_Mush;
}

bool Rabbit_canBePlacedByLoc(GameCore* gameCore, int cellX, int cellY, void* optional)
{
    for (int x = 0; x < MAX_RABBITS + MAX_FOXES + MAX_MUSHROOMS; x++)
    {
        Rabbit* l_Rabb = &gameCore->Rabbits[x];

        if (!l_Rabb || l_Rabb == optional) continue;

        if (cellX < 0 || cellX >= GAME_GRID_SIZE || cellY < 0 || cellY >= GAME_GRID_SIZE)
        {
            return false;
        }

        bool l_Other = false;
        if (l_Rabb->Type == FOX)
        {
            Vec2 l_OtherCell = Fox_getSecondCell(l_Rabb);
            l_Other = l_OtherCell.x == cellX && l_OtherCell.y == cellY;
        }

        if ((l_Rabb->CellX == cellX && l_Rabb->CellY == cellY) || l_Other)
        {
            return false;
        }

    }

    return true;
}

bool Rabbit_canBePlaced(GameCore* scene, Rabbit* rabb)
{
    if (rabb->Type == FOX)
    {
        Vec2 otherCell = Fox_getSecondCell(rabb);
        return Rabbit_canBePlacedByLoc(scene, rabb->CellX, rabb->CellY, rabb) && Rabbit_canBePlacedByLoc(scene, otherCell.x, otherCell.y, rabb);
    }
    else
    {
        return Rabbit_canBePlacedByLoc(scene, rabb->CellX, rabb->CellY, rabb);
    }
}

void GameCore_deletePiece(GameCore* gameCore, Rabbit* rabb)
{
    bool l_Passed = false;

    int baseIndex = -1;
    int* count = 0;

    switch (rabb->Type)
    {
    case RABBIT:
        count = &RABBIT_COUNT;
        baseIndex = 0;
        break;
    case FOX:
        count = &FOX_COUNT;
        baseIndex = MAX_RABBITS;
        break;
    case MUSHROOM:
        count = &MUSHROOM_COUNT;
        baseIndex = MAX_RABBITS + MAX_FOXES;
        break;
    default: break;
    }

    for (int x = baseIndex; x < baseIndex + *count; x++)
    {
        Rabbit* l_Index = RABBIT(x);

        if (l_Index->CellX == rabb->CellX && l_Index->CellY == rabb->CellY && l_Index->Type != NONE)
        {
            l_Index->Type = NONE;
            l_Index->CellX = -1;
            l_Index->CellY = -1;

            l_Passed = true;
            continue;
        }

        if (l_Passed)
        {
            //printf("\nPassed\n");
            if (x - 1 >= baseIndex && x - 1 < baseIndex + *count)
            {
                gameCore->Rabbits[x - 1] = *l_Index;
            }
        }
    }
    
    if (l_Passed && baseIndex >= 0 && count)
    {
        *count -= 1;
        gameCore->Rabbits[baseIndex + *count].Type = NONE;
        gameCore->Rabbits[baseIndex + *count].CellX = -1;
        gameCore->Rabbits[baseIndex + *count].CellY = -1;
    }
}

bool GameCore_isAimingRabbit(GameCore* gameCore, int cellX, int cellY, Rabbit** res)
{
    for (int x = 0; x < MAX_RABBITS + MAX_FOXES + MAX_MUSHROOMS; x++)
    {
        Rabbit* l_Rabb = RABBIT(x);

        if (l_Rabb->Type == NONE) continue;

        bool l_OtherCond = false;
        if (l_Rabb->Type == FOX)
        {
            Vec2 l_Pos = Fox_getSecondCell(l_Rabb);
            l_OtherCond = l_Pos.x == cellX && l_Pos.y == cellY;
        }

        if (l_Rabb->CellX == cellX && l_Rabb->CellY == cellY || l_OtherCond)
        {
            *res = l_Rabb;
            return true;
        }
    }

    *res = NULL;
    return false;
}
