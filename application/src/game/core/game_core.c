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

    if (targetX < 0 || targetX >= GAME_GRID_SIZE || targetY < 0 || targetY >= GAME_GRID_SIZE) return false;

    int directionX = signOf(rabbit->CellX - targetX);
    int directionY = signOf(rabbit->CellY - targetY);

    int itCount = 0;

    do
    {
        indexX -= directionX;
        indexY -= directionY;

        EObjectType l_Type = Scene_getObjTypeAtLocation(scene, indexX, indexY);

        if (l_Type == NO_OBJECT && itCount == 0) return false;

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
