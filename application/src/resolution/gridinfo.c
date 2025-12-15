#include "gridinfo.h"

int GetRabbitNumber()
{
    int res;
    return res;
}


Vec2Int GetRabbitPosition(Rabbit* rabbit)
{
    Vec2Int pos;
    pos.x = rabbit->CellX;
    pos.y = rabbit->CellY;
    return pos;
}


bool isSameGrid(GridInfo* grid1, GridInfo* grid2)
{
    assert(grid1);
    assert(grid2);

    for (int i = 0; i < grid1->Fnbr; i++)
    {
        if (grid1->Fpos[i].x != grid2->Fpos[i].x ||
            grid1->Fpos[i].y != grid2->Fpos[i].y)
        {
            return false;
        }
    }
    for (int i = 0; i < grid1->Rnbr; i++)
    {
        bool isInG2 = false;
        for (int j = 0; j < grid1->Rnbr; j++)
        {
            if (grid1->Rpos[i].x == grid2->Rpos[j].x &&
                grid1->Rpos[i].y == grid2->Rpos[j].y)
            {
                isInG2 = true;
            }
        }
        if (!isInG2)
            return false;
    }
    return true;
}
