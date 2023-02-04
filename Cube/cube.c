#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>

#include "subcube.c"

// TODO: try splitting into a 2x2x2 of corners and something for sides?
// typedef Subcube Cube[20];

int NUM_CORNERS = 8;
int NUM_SIDES = 12;

Subcube null = -1;

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

Subcube* subcube(Cube* cube, size_t i, size_t j, size_t k);

Cube* init_Cube();

void destroy_Cube(Cube* cube);

Cube* randomize_Cube();

void print_subcube_face(Cube* cube, size_t i, size_t j, size_t k, enum axis axis);

void print_top_or_bottom(Cube* cube, size_t layer, size_t row);

void print_center_row(Cube* cube, size_t layer, size_t row);

void print_cube_line(Cube* cube, size_t row, void(*printer)(Cube* cube, size_t layer, size_t row));

void print_Cube(Cube* cube);

void print_binary(char c);

void print_cube_vals(Cube* cube);

Cube* rotate_Cube(Cube* cube, face side, rotation rot);

int cmp_cubes(Cube state1, Cube state2);

void pos(face face, size_t index, size_t ordering[4][2], size_t pos[3]);



Subcube* subcube(Cube* cube, size_t i, size_t j, size_t k)
{
    switch (subcubeType(i, j, k))
    {
    case CORNER:
        return (*cube + i * 2 + j + k / 2);
    case SIDE:
        return (*cube + NUM_CORNERS + subcubeIndex(i, j, k) + 1);
    default:
        return &null;
    }
    // return (*cube + i * 9 + j * 3 + k);
}

Cube* init_Cube()
{

    size_t dim = 3;
    Cube* cube = malloc(sizeof(Cube));

    for (size_t i = 0; i < dim; i++)
    {

        for (size_t j = 0; j < dim; j++)
        {

            for (size_t k = 0; k < dim; k++)
            {
                if (subcube(cube, i, j, k) != &null)
                    *subcube(cube, i, j, k) = defaultCubeAt(i, j, k);
            }
        }
    }

    return cube;
}

Cube* copy(Cube* cube) {
    size_t dim = 3;
    Cube* newCube = malloc(sizeof(Cube));

    for (size_t i = 0; i < dim; i++)
    {
        for (size_t j = 0; j < dim; j++)
        {
            for (size_t k = 0; k < dim; k++)
            {
                if (subcube(cube, i, j, k) != &null)
                    *subcube(newCube, i, j, k) = *subcube(cube, i, j, k);
            }
        }
    }

    return newCube;
}

void destroy_Cube(Cube* cube) {
    free(cube);
}

Cube* randomize_cube() {
    Cube* cube = init_Cube();
    return cube;
}

void print_subcube_face(Cube* cube, size_t i, size_t j, size_t k, enum axis axis) {
    face faceColor = colorAlongAxis(*subcube(cube, i, j, k), axis, subcubeType(i, j, k));
    printf("%c ", faceToChar(faceColor));
}

void print_top_or_bottom(Cube* cube, size_t layer, size_t row) {
    printf("  ");
    for (size_t i = 0; i < 3; i++)
    {
        print_subcube_face(cube, layer, row, i, BG);
    }
    printf("  ");
}

void print_center_row(Cube* cube, size_t layer, size_t row) {
    print_subcube_face(cube, layer, row, 0, OR);

    for (size_t j = 0; j < 3; j++)
    {
        print_subcube_face(cube, layer, row, j, WY);
    }

    print_subcube_face(cube, layer, row, 2, OR);
}

void print_cube_line(Cube* cube, size_t row, void(*printer)(Cube* cube, size_t layer, size_t row)) {
    for (size_t i = 0; i < 3; i++)
    {
        printer(cube, i, row);
        printf("\t");
    }
    puts("");
}

void print_Cube(Cube* cube) {
    print_cube_line(cube, 0, print_top_or_bottom);
    print_cube_line(cube, 0, print_center_row);
    print_cube_line(cube, 1, print_center_row);
    print_cube_line(cube, 2, print_center_row);
    print_cube_line(cube, 2, print_top_or_bottom);
    puts("");
}

void print_binary(char c) {
    if (c == null)
        return;
    for (int i = 7; i >= 0; --i)
    {
        putchar((c & (1 << i)) ? '1' : '0');
    }
    printf("\t");

}

//prints the values and their binary for each subcube; useful for debugging
void print_cube_vals(Cube* cube) {
    for (size_t i = 0; i < 3; i++)
    {
        for (size_t j = 0; j < 3; j++)
        {
            for (size_t k = 0; k < 3; k++)
            {
                printf("%d: ", *subcube(cube, i, j, k));
                print_binary(*subcube(cube, i, j, k));
            }
            puts("");
        }
        puts("");
    }

}

//used to get the next position in a sequence when rotating along a face.
void pos(face face, size_t index, size_t ordering[4][2], size_t pos[3]) {
    enum axis ax = faceToAxis(face);
    size_t firstIndex = (ax == WY ? 1 : 0);
    size_t secondIndex = (ax == OR ? 1 : 2);

    pos[ax] = (face < 3 ? 0 : 2);
    pos[firstIndex] = ordering[index][0];
    pos[secondIndex] = ordering[index][1];
}


Cube* rotate_Cube(Cube* cube, face side, rotation rot) {
    size_t shift = rot;
    if (shift == 3) shift == -1;
    if (side == YELLOW || side == BLUE || side == RED) shift *= -1;

    Cube* newCube = copy(cube);

    //shift the correct faces
    size_t nextIndex = shift % 4;
    size_t curr_c[3], next_c[3], curr_s[3], next_s[3];

    pos(side, 0, cornerRotationOrder, curr_c);
    pos(side, nextIndex, cornerRotationOrder, next_c);
    pos(side, 0, sideRotationOrder, curr_s);
    pos(side, nextIndex, sideRotationOrder, next_s);

    for (size_t i = 0; i < 4; i++)
    {
        char* newCorner = subcube(cube, curr_c[0], curr_c[1], curr_c[2]);
        *subcube(newCube, next_c[0], next_c[1], next_c[2]) = rotateSubcube(newCorner, CORNER, rot, side);

        char* newSide = subcube(cube, curr_s[0], curr_s[1], curr_s[2]);
        *subcube(newCube, next_s[0], next_s[1], next_s[2]) = rotateSubcube(newSide, SIDE, rot, side);

        if (i == 1 && rot == ROT_180) {
            nextIndex = 3;
            pos(side, 1, cornerRotationOrder, curr_c);
            pos(side, 1, sideRotationOrder, curr_s);
        }
        else {
            nextIndex = (nextIndex + shift) % 4;
            for (size_t i = 0; i < 3; i++)
            {
                curr_c[i] = next_c[i];
                curr_s[i] = next_s[i];
            }
        }

        pos(side, nextIndex, cornerRotationOrder, next_c);
        pos(side, nextIndex, sideRotationOrder, next_s);

    }

    return newCube;
}

int cmp_cubes(Cube s1, Cube s2)
{
    // char* s1 = (char*)cube1;
    // char* s2 = (char*)cube2;
    for (size_t i = 0; i <= 20; ++i)
        if (s1[i] != s2[i])
            return s1[i] - s2[i];
    return 0;
}