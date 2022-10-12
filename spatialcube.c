#include <stdlib.h>
#include <stdio.h>
#include <math.h>
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
    return &((*cube)[i * 9 + j * 3 + k]);
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

void print_subcube_face(sCube* cube, size_t i, size_t j, size_t k, enum axis axis) {
    face faceColor = colorAlongAxis(*subcube(cube, i, j, k), axis, subcubeType(i, j, k));
    printf("%c\t", faceToChar(faceColor));
}

void print_cube_layer(sCube* cube, size_t layer)
{
    printf("\t");
    for (size_t i = 0; i < 3; i++)
    {
        print_subcube_face(cube, layer, 0, i, BG);
    }
    puts("");

    for (size_t i = 0; i < 3; i++)
    {
        print_subcube_face(cube, layer, i, 0, OR);

        for (size_t j = 0; j < 3; j++)
        {
            print_subcube_face(cube, layer, i, j, WY);
        }

        print_subcube_face(cube, layer, i, 2, OR);
        puts("");
    }

    printf("\t");
    for (size_t i = 0; i < 3; i++)
    {
        print_subcube_face(cube, layer, 2, i, BG);
    }
    puts("");

}

void print_Cube(sCube* cube) {
    for (size_t i = 0; i < 3; i++)
    {
        print_cube_layer(cube, i);
        puts("\n");
    }

}

void print_binary(char c) {
    for (int i = 7; i >= 0; --i)
    {
        putchar((c & (1 << i)) ? '1' : '0');
    }
    printf("\t");

}

void print_cube_vals(sCube* cube) {
    for (size_t i = 0; i < 3; i++)
    {
        for (size_t j = 0; j < 3; j++)
        {
            for (size_t k = 0; k < 3; k++)
            {
                printf("%d: ", *subcube(cube, i, j, k));
                print_binary(*subcube(cube, i, j, k));
                // printf("%d\t", *subcube(cube, i, j, k));
            }
            puts("");
        }
        puts("");
    }

}

int main(int argc, char const* argv[])
{
    sCube* cube = init_Cube();
    // print_cube_vals(cube);
    // puts("");
    print_Cube(cube);
    // puts("");
    // print_cube_vals(cube);
    return 0;
}
