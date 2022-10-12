#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>

// #include "cube_constants.c"
#include "spatial_subcube.c"

// TODO: try splitting into a 2x2x2 of corners and something for sides?
typedef char sCube[27];

size_t cornerRotationOrder[4][2] = {
    {0,0},
    {0,2},
    {2,2},
    {2,0}
};

size_t sideRotationOrder[4][2] = {
    {0,1},
    {1,2},
    {2,1},
    {1,0},
};

char* subcube(sCube* cube, size_t i, size_t j, size_t k);

sCube* init_Cube();

void destroy_Cube(sCube* cube);

sCube* randomize_Cube();

char faceToChar(face face);

void print_subcube_face(sCube* cube, size_t i, size_t j, size_t k, enum axis axis);

void print_cube_layer(sCube* cube, size_t layer);

void print_Cube(sCube* cube);

void print_binary(char c);

void print_cube_vals(sCube* cube);

sCube* rotate_Cube(sCube* cube, face side, rotation rot);



char* subcube(sCube* cube, size_t i, size_t j, size_t k)
{
    return (*cube + i * 9 + j * 3 + k);
}

sCube* init_Cube()
{

    size_t dim = 3;
    sCube* cube = malloc(sizeof(sCube));

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

sCube* copy(sCube* cube) {
    size_t dim = 3;
    sCube* newCube = malloc(sizeof(sCube));

    for (size_t i = 0; i < dim; i++)
    {

        for (size_t j = 0; j < dim; j++)
        {

            for (size_t k = 0; k < dim; k++)
            {
                *subcube(newCube, i, j, k) = *subcube(cube, i, j, k);
            }
        }
    }

    return newCube;
}

//returns a character representing that specific face
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

//displays the cube in a layered form
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

//prints the values and their binary for each subcube; useful for debugging
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

size_t* pos(face face, size_t index, size_t ordering[4][2]) {
    size_t* pos = calloc(3, sizeof(size_t));

    enum axis ax = faceToAxis(face);
    size_t firstIndex = (ax == WY ? 1 : 0);
    size_t secondIndex = (ax == OR ? 1 : 2);

    pos[ax] = (face < 3 ? 0 : 2);
    pos[firstIndex] = ordering[index][0];
    pos[secondIndex] = ordering[index][1];
    return pos;
}


sCube* rotate_Cube(sCube* cube, face side, rotation rot) {
    size_t shift = rot;
    if (shift == 3) shift == -1;
    if (side == YELLOW || side == BLUE || side == RED) shift *= -1;

    sCube* newCube = copy(cube);

    //shift the correct faces
    size_t nextIndex = shift % 4;
    size_t* curr_c = pos(side, 0, cornerRotationOrder);
    size_t* next_c = pos(side, nextIndex, cornerRotationOrder);
    size_t* curr_s = pos(side, 0, sideRotationOrder);
    size_t* next_s = pos(side, nextIndex, sideRotationOrder);
    for (size_t i = 0; i < 4; i++)
    {
        char* newCorner = subcube(cube, curr_c[0], curr_c[1], curr_c[2]);
        *subcube(newCube, next_c[0], next_c[1], next_c[2]) = rotateSubcube(newCorner, CORNER, rot, side);

        char* newSide = subcube(cube, curr_s[0], curr_s[1], curr_s[2]);
        *subcube(newCube, next_s[0], next_s[1], next_s[2]) = rotateSubcube(newSide, SIDE, rot, side);

        free(curr_c);
        free(curr_s);

        if (i == 1 && rot == ROT_180) {
            nextIndex = 3;
            curr_c = pos(side, 1, cornerRotationOrder);
            curr_s = pos(side, 1, sideRotationOrder);
        }
        else {
            nextIndex = (nextIndex + shift) % 4;
            curr_c = next_c;
            curr_s = next_s;
        }

        next_c = pos(side, nextIndex, cornerRotationOrder);
        next_s = pos(side, nextIndex, sideRotationOrder);

    }

    return newCube;
}

int main(int argc, char const* argv[])
{
    sCube* cube = init_Cube();
    sCube* newCube = rotate_Cube(cube, GREEN, ROT_180);

    print_Cube(newCube);
    return 0;
}