#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

// #include "cube_constants.c"
#include "spatial_subcube.c"

// TODO: try splitting into a 2x2x2 of corners and something for sides?
typedef char sCube[27];

char* subcube(sCube* cube, size_t i, size_t j, size_t k);

sCube* init_Cube();

void destroy_Cube(sCube* cube);

sCube* randomize_Cube();

char faceToChar(face face);

void print_Cube(sCube* cube);

sCube* rotate_Cube(sCube* cube, enum faces side, enum rotations rot);

char* subcube(sCube* cube, size_t i, size_t j, size_t k)
{
    return &(*cube[i * 9 + j * 3 + k]);
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

char faceToChar(face face) {
    switch (face)
    {
    case WHITE:
        return 'w';
    case BLUE:
        return 'b';
    case ORANGE:
        return 'o';
    case YELLOW:
        return 'y';
    case GREEN:
        return 'g';
    case RED:
        return 'r';
    case BLANK:
        return 'X';
    }
}

void print_Cube(sCube* cube)
{
    printf("\t");
    for (size_t n = 0; n < 3; n++)
    {
        face faceColor = colorAlongAxis(*subcube(cube, 0, 0, n), BG, subcubeType(0, 0, n));
        printf("%c\t", faceToChar(faceColor));
    }
    puts("");

    for (size_t i = 0; i < 3; i++)
    {
        /* code */
    }


}

int main(int argc, char const* argv[])
{
    sCube* cube = init_Cube();
    print_Cube(cube);
    // printf("%d\n", (-2 & 7));
    return 0;
}
