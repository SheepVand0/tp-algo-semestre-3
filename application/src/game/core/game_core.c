/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "game_core.h"
#include "game/scene.h"

Rabbit* Rabbit_create(Scene* scene, int cellX, int cellY)
{
    Rabbit* l_Rabbit = malloc(sizeof(Rabbit));

    assert(l_Rabbit);
    l_Rabbit->CellX = cellX;
    l_Rabbit->CellY = cellY;

    AssetManager* assets = Scene_getAssetManager(scene);
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

bool Rabbit_move(Rabbit* rabbit, Scene* scene, int targetX, int targetY)
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

        EObjectType l_Type = Scene_getObjTypeAtLocation(scene, indexX, indexY);

        if (rabbit->Type == FOX)
        {
            if (l_Type == NO_OBJECT)
            {
                rabbit->CellX -= l_ToSubX;
                rabbit->CellY -= l_ToSubY;
                return true;
            }
        }

        if ((l_Type == NO_OBJECT && itCount == 0) || rabbit->Type != RABBIT) return false;

        itCount++;

        if (l_Type == NO_OBJECT)
        {
            rabbit->CellX = indexX;
            rabbit->CellY = indexY;
            return true;
        }
    } while (true);

    return true;
}

Rabbit* Fox_create(Scene* scene, int cellX0, int cellY0, ERabbitDirection direction)
{
    Rabbit* l_Fox = malloc(sizeof(Rabbit));

    assert(l_Fox);
    l_Fox->CellX = cellX0;
    l_Fox->CellY = cellY0;
    l_Fox->Direction = direction;

    AssetManager* assets = Scene_getAssetManager(scene);
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
