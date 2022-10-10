#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

// #include "cube_constants.c"
#include "spatial_subcube.c"

// TODO: try splitting into a 2x2x2 of corners and one of sides?
typedef char sCube[27];

char* subcube(sCube* cube, size_t i, size_t j, size_t k);

sCube* init_Cube();

void destroy_Cube(sCube* cube);

sCube* randomize_Cube();

void print_Cube(sCube* cube);

sCube* rotate_Cube(sCube* cube, enum faces side, enum rotations rot);

char* subcube(sCube* cube, size_t i, size_t j, size_t k)
{
    return &(*cube[i * 9 + j * 3 + k]);
}

char getID(size_t i, size_t j, size_t k, char* nextCorner, char* nextSide)
{
    char id = -1;
    enum cubeTypes type = subcubeType(i, j, k);
    if (type == SIDE)
    {
        id = *nextSide;
        *nextSide = id + 1;
        // printf("%d\n", *nextSide);
    }
    else if (type == CORNER)
    {
        id = *nextCorner;
        *nextCorner = id + 1;
    }
    else if (type == CENTER)
    {
        id = 0;
    }

    return id;
}

sCube* init_Cube()
{

    size_t dim = 3;
    sCube* cube = malloc(sizeof(sCube));

    char cornerID = 0, sideID = 0;

    for (size_t i = 0; i < dim; i++)
    {

        for (size_t j = 0; j < dim; j++)
        {

            for (size_t k = 0; k < dim; k++)
            {
                *subcube(cube, i, j, k) = defaultCubeAt(i, j, k);
            }
        }
    }

    return cube;
}

void print_Cube(sCube* cube)
{
    for (size_t i = 0; i < 27; i++)
    {
        printf("%d\t", *cube[i]);

        if (i % 3 == 2)
            puts("");
        if (i % 9 == 8)
            puts("");
    }
}

int main(int argc, char const* argv[])
{
    // sCube* cube = init_Cube();
    // print_Cube(cube);
    printf("%d\n", (-2 & 7));
    return 0;
}
