#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "cube_constants.c"

typedef struct spatialCube
{
    char ***cubes;
} sCube;

sCube *init_Cube();

void destroy_Cube(sCube *cube);

sCube *randomize_Cube();

void print_Cube(sCube *cube);

sCube *rotate_Cube(sCube *cube, enum faces side, enum rotations rot);

bool isCenter(size_t i, size_t j, size_t k)
{
    size_t numCenter = 0;
    if (i == 1)
        numCenter++;
    if (j == 1)
        numCenter++;
    if (k == 1)
        numCenter++;

    return numCenter > 1;
}

bool isSide(size_t i, size_t j, size_t k)
{
    return (i == 1 || j == 1 || k == 1) && !isCenter(i, j, k);
}

bool isCorner(size_t i, size_t j, size_t k)
{
    return i != 1 && j != 1 && k != 1;
}

char getID(size_t i, size_t j, size_t k, char *nextCorner, char *nextSide)
{
    char id = 0;
    if (isSide(i, j, k))
    {
        id = *nextSide;
        *nextSide++;
        printf("%d\n", *nextSide);
    }
    else if (isCorner(i, j, k))
    {
        id = *nextCorner;
        *nextCorner++;
    }

    return id;
}

sCube *init_Cube()
{

    size_t dim = 3;
    sCube *cube = malloc(sizeof(sCube));
    cube->cubes = calloc(3, sizeof(char));

    char cornerID = 0, sideID = 0;

    for (size_t i = 0; i < dim; i++)
    {
        cube->cubes[i] = calloc(3, sizeof(char));

        for (size_t j = 0; j < dim; j++)
        {
            cube->cubes[i][j] = calloc(3, sizeof(char));

            for (size_t k = 0; k < dim; k++)
            {
                cube->cubes[i][j][k] = getID(i, j, k, &cornerID, &sideID);
                //  if (isCenter(i, j, k))
                // {
                // cube->cubes[i][j][k] = 0;
                continue;
                // }
                // else if (isSide(i, j, k))
                // {
                // cube->cubes[i][j][k] = sideID--;
                // }
                // else if (isCorner(i, j, k))
                // {
                // cube->cubes[i][j][k] = cornerID++;
                // }
                // else cube->cubes[i][j][k] = 'h';

                printf("%d\t", cube->cubes[i][j][k]);
            }
            puts("");
        }
        puts("");
    }

    return cube;
}

int main(int argc, char const *argv[])
{
    init_Cube();
    return 0;
}
