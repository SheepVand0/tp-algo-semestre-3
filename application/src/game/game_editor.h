#pragma once

 

typedef struct Rabbit Rabbit;

typedef struct GameEditor
{
    Rabbit* AddingObject;

} GameEditor;

extern GameEditor g_gameEditor;

Rabbit* GameEditor_buildUsableArray(Rabbit* rabbits);
