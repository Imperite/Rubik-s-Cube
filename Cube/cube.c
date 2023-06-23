/**
 * @file cube.c
 * @author David  B (dsbradle)
 *
 * Implementation of the cube as a 20-byte array. Each subcube is represented as one byte.
 *
 */
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>

#include "cube.h"
#include "subcube.c"


// Size of the cube being stored
const size_t CUBE_STORAGE_SIZE = 20;

const size_t CUBE_PRINT_SIZE = 171;

/**
 * Current implementation of cube - just a 20-byte array.
 */
struct cube {
    char cubes[20];
};

/** Null subcube*/
const Subcube null = -1;

/**Array used for knowing where the rotation goes to next for corners*/
size_t cornerRotationOrder[4][2] = {
    {0,0},
    {0,2},
    {2,2},
    {2,0}
};

/**Array used for knowing where the rotation goes to next for sides*/
size_t sideRotationOrder[4][2] = {
    {0,1},
    {1,2},
    {2,1},
    {1,0},
};

/**
 * Returns a pointer to the location of the subcube in the cube structure based on a 3D coordinate.
 * @param cube  the cube the subcube is in
 * @param i the x-coordinate of the subcube
 * @param j the y-coordinate of the subcube
 * @param k the z-coordinate of the subcube
 * @return Subcube*  the pointer to the subcube in memory
 */
Subcube* subcube(const Cube* cube, size_t i, size_t j, size_t k);


/**
 * Adds the face color of the specified subcube to the output
 * @param cube the cube to pull from
 * @param output the output to place to
 * @param cursor the cursor to place the color at
 * @param i the x coord of the subcube
 * @param j the y coord of the subcube
 * @param k the z coord of the subcube
 * @param axis the axis to get the color of the subcube from
 */
void subcube_print_face(const Cube* cube, char* output, size_t* cursor, size_t i, size_t j, size_t k, enum axis axis);

/**
 * Prints the top or bottom surface of a cube slice
 * @param cube the cube to pull from
 * @param output the output to print into
 * @param cursor the index to place the output at
 * @param layer the layer of the cube being printed
 * @param row the row of the cube to print
 */
void print_top_or_bottom(const Cube* cube, char* output, size_t* cursor, size_t layer, size_t row);

/**
 * Prints a center row of the cube slice
 * @param cube the cube to pull from
 * @param output the output to print into
 * @param cursor the index to place the output at
 * @param layer the layer of the cube being printed
 * @param row the row of the cube to print
 */
void print_center_row(const Cube* cube, char* output, size_t* cursor, size_t layer, size_t row);

/**
 * Prints a line of all slices to the output.
 * @param cube the cube to pull from
 * @param output the output to print to
 * @param cursor the index to place the output at
 * @param row the row to print
 * @param printer_func the function to use for printing
 */
void cube_print_line(const Cube* cube, char* output, size_t* cursor, size_t row, void(*printer_func)(const Cube* cube, char* output, size_t* cursor, size_t layer, size_t row));

/**
 * Prints the binary form of the character(byte)
 * @param c the byte to print out
 */
void print_binary(char c);

/**
 * Used to get the next position in a sequence when rotating along a face.
 * @param face the face rotating along
 * @param index  the current index of the subcube being rotated
 * @param ordering the ordering used for rotation
 * @param pos the position to return
 */
void pos(face face, size_t index, size_t ordering[4][2], size_t pos[3]);



Subcube* subcube(const Cube* cube, size_t i, size_t j, size_t k)
{
    switch (subcube_type(i, j, k)) {
    case CORNER:
        return (cube->cubes + i * 2 + j + k / 2);
    case SIDE:
        return (cube->cubes + NUM_CORNERS + subcube_index(i, j, k));
    default:
        return &null;
    }
    // return (cube->cubes + i * 9 + j * 3 + k);
}

void cube_create(Cube* cube)
{
    size_t dim = 3;
    for (size_t i = 0; i < dim; i++) {

        for (size_t j = 0; j < dim; j++) {

            for (size_t k = 0; k < dim; k++) {
                if (subcube(cube, i, j, k) != &null)
                    *subcube(cube, i, j, k) = subcube_default_at(i, j, k);
            }
        }
    }
}

void cube_copy(const Cube* original, Cube* copy)
{
    size_t dim = 3;

    for (size_t i = 0; i < dim; i++) {
        for (size_t j = 0; j < dim; j++) {
            for (size_t k = 0; k < dim; k++) {
                if (subcube(original, i, j, k) != &null)
                    *subcube(copy, i, j, k) = *subcube(original, i, j, k);
            }
        }
    }
}

void cube_free(Cube* cube)
{
    free(cube);
}

void cube_randomized(Cube* cube)
{
    cube_create(cube);
    //TODO: something, idk yet
}

void subcube_print_face(const Cube* cube, char* output, size_t* cursor, size_t i, size_t j, size_t k, enum axis axis)
{
    face faceColor = subcube_color_along_axis(*subcube(cube, i, j, k), axis, subcube_type(i, j, k));
    output[(*cursor)++] = faceToChar(faceColor);
    output[(*cursor)++] = ' ';
    // printf("%c ", faceToChar(faceColor));
}

void print_top_or_bottom(const Cube* cube, char* output, size_t* cursor, size_t layer, size_t row)
{
    output[(*cursor)++] = ' ';
    output[(*cursor)++] = ' ';
    // printf("  ");
    for (size_t i = 0; i < 3; i++) {
        subcube_print_face(cube, output, cursor, layer, row, i, BG);
    }
    output[(*cursor)++] = ' ';
    output[(*cursor)++] = ' ';
    // printf("  ");
}

void print_center_row(const Cube* cube, char* output, size_t* cursor, size_t layer, size_t row)
{
    subcube_print_face(cube, output, cursor, layer, row, 0, OR);

    for (size_t j = 0; j < 3; j++) {
        subcube_print_face(cube, output, cursor, layer, row, j, WY);
    }

    subcube_print_face(cube, output, cursor, layer, row, 2, OR);
}

void cube_print_line(const Cube* cube, char* output, size_t* cursor, size_t row, void(*printer_func)(const Cube* cube, char* output, size_t* cursor, size_t layer, size_t row))
{
    for (size_t i = 0; i < 3; i++) {
        printer_func(cube, output, cursor, i, row);
        output[(*cursor)++] = '\t';
    }
    output[(*cursor)++] = '\n';
}

void cube_string(char dest[CUBE_PRINT_SIZE], const Cube* cube)
{
    size_t cursor = 0;
    cube_print_line(cube, dest, &cursor, 0, print_top_or_bottom);
    cube_print_line(cube, dest, &cursor, 0, print_center_row);
    cube_print_line(cube, dest, &cursor, 1, print_center_row);
    cube_print_line(cube, dest, &cursor, 2, print_center_row);
    cube_print_line(cube, dest, &cursor, 2, print_top_or_bottom);
    dest[cursor++] = '\0';
}


void cube_print(const Cube* cube)
{
    char output[CUBE_PRINT_SIZE];
    cube_string(output, cube);
    puts(output);
}

void print_binary(char c)
{
    if (c == null)
        return;
    for (int i = 7; i >= 0; --i) {
        putchar((c & (1 << i)) ? '1' : '0');
    }
    printf("\t");

}

//prints the values and their binary for each subcube; useful for debugging
void cube_print_vals(const Cube* cube)
{
    for (size_t i = 0; i < 3; i++) {
        for (size_t j = 0; j < 3; j++) {
            for (size_t k = 0; k < 3; k++) {
                printf("%d: ", *subcube(cube, i, j, k));
                print_binary(*subcube(cube, i, j, k));
            }
            puts("");
        }
        puts("");
    }

}

void pos(face face, size_t index, size_t ordering[4][2], size_t pos[3])
{
    enum axis ax = faceToAxis(face);
    size_t firstIndex = (ax == WY ? 1 : 0);
    size_t secondIndex = (ax == OR ? 1 : 2);

    pos[ax] = (face < 3 ? 0 : 2);
    pos[firstIndex] = ordering[index][0];
    pos[secondIndex] = ordering[index][1];
}


void cube_rotate(const Cube* cube, Cube* newCube, face side, rotation rot)
{
    size_t shift = rot;
    if (shift == 3) shift = -1;
    if (side == YELLOW || side == BLUE || side == RED) shift *= -1;

    cube_copy(cube, newCube);

    //shift the correct faces
    size_t nextIndex = shift % 4;
    size_t curr_c[3], next_c[3], curr_s[3], next_s[3];

    pos(side, 0, cornerRotationOrder, curr_c);
    pos(side, nextIndex, cornerRotationOrder, next_c);
    pos(side, 0, sideRotationOrder, curr_s);
    pos(side, nextIndex, sideRotationOrder, next_s);

    for (size_t i = 0; i < 4; i++) {
        char* newCorner = subcube(cube, curr_c[0], curr_c[1], curr_c[2]);
        *subcube(newCube, next_c[0], next_c[1], next_c[2]) = subcube_rotate(newCorner, CORNER, rot, side);

        char* newSide = subcube(cube, curr_s[0], curr_s[1], curr_s[2]);
        *subcube(newCube, next_s[0], next_s[1], next_s[2]) = subcube_rotate(newSide, SIDE, rot, side);

        if (i == 1 && rot == ROT_180) {
            nextIndex = 3;
            pos(side, 1, cornerRotationOrder, curr_c);
            pos(side, 1, sideRotationOrder, curr_s);
        }
        else {
            nextIndex = (nextIndex + shift) % 4;
            for (size_t i = 0; i < 3; i++) {
                curr_c[i] = next_c[i];
                curr_s[i] = next_s[i];
            }
        }

        pos(side, nextIndex, cornerRotationOrder, next_c);
        pos(side, nextIndex, sideRotationOrder, next_s);

    }

    return newCube;
}

int cube_compare(const Cube* s1, const Cube* s2)
{
    for (size_t i = 0; i < 20; ++i)
        if (s1->cubes[i] != s2->cubes[i])
            return s1->cubes[i] - s2->cubes[i];
    return 0;
}

size_t cube_hash(const Cube* s1)
{
    size_t val = 0;
    for (size_t i = 0; i < 20; ++i)
        val += s1->cubes[i];
    return val;
}