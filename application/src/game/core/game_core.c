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



Obstacle* Fox_create(Scene* scene, int cellX0, int cellY0, int cellX1, int cellY1)
{
    Obstacle* l_Fox = malloc(sizeof(Obstacle));

    assert(l_Fox);
    l_Fox->CellX0 = cellX0;
    l_Fox->CellY0 = cellY0;
    l_Fox->CellX1 = cellY1;
    l_Fox->CellY1 = cellY1;

    AssetManager* assets = Scene_getAssetManager(scene);
    SpriteSheet* spriteSheet = AssetManager_getSpriteSheet(assets, SPRITE_GAME);
    AssertNew(spriteSheet);
    l_Fox->ObstacleSprite = SpriteSheet_getGroupByName(spriteSheet, "fox");
    AssertNew(l_Fox->ObstacleSprite);

    return l_Fox;
}

void Obstacle_destroy(Obstacle* obstacle)
{
    free(obstacle);
}

SelectedObject* SelectedObject_create(void* object, EObjectType type)
{
    SelectedObject* l_Obj = malloc(sizeof(SelectedObject));

    assert(l_Obj);

    switch (type)
    {
        case RABBIT:
            l_Obj->SelectedRabbit = (Rabbit*)object;
            break;
        case OBSTACLE:
            l_Obj->SelectedObstacle = (Obstacle*)object;
            break;
        default: break;
    }

    l_Obj->ObjectType = type;
    return l_Obj;
}

void SelectedObject_destroy(SelectedObject* obj)
{
    free(obj);
}
