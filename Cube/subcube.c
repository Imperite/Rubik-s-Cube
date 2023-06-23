/**
 * @file subcube.c
 * @author David  B (dsbradle)
 *
 * Methods and data used for handling subcubes when represented as a single byte.
 */
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "subcube.h"

face sideIDtoFaces[13][2] = {
    {WHITE, BLUE},
    {WHITE, ORANGE},
    {WHITE, RED},
    {WHITE, GREEN},

    {BLUE, ORANGE},
    {BLUE, RED},
    {BLANK, BLANK},
    {GREEN, ORANGE},
    {GREEN, RED},

    {YELLOW, BLUE},
    {YELLOW, ORANGE},
    {YELLOW, RED},
    {YELLOW, GREEN}
};


/** Null subcube*/
const Subcube null = -1;


size_t subcube_index(size_t i, size_t j, size_t k)
{
    size_t sum = 9 * i + 3 * j + k;
    size_t result = (sum + 1) / 2 - 1;
    if (result > 6) result--;
    return result;
}

cubeType subcube_type(size_t i, size_t j, size_t k)
{
    cubeType numCenter = 0;
    if (i == 1)
        numCenter++;
    if (j == 1)
        numCenter++;
    if (k == 1)
        numCenter++;

    return numCenter;
}

bool* subcube_rotations(const Subcube subcube)
{
    bool* rots = calloc(4, sizeof(bool));
    for (size_t i = 0; i < 4; i++) {
        rots[i] = 1 & (subcube >> (i + 4));
    }

    return rots;
}


face* subcube_colors(const Subcube subcube)
{
    face* colors = calloc(3, sizeof(face));

    // because the value for the axis (and i) aligns with the value for the default colors (WBO),  can just add 3 if that bit is true to shift to the opposite facing color
    for (size_t i = 0; i < 3; i++) {
        bool rot = subcube & (1 << i);
        colors[i] = i + (rot * 3);
    }

    return colors;
}


face subcube_color_along_axis(Subcube subcube, enum axis axis, cubeType type)
{
    // printf("%d %d %d\t", subcube, axis, type);
    if (type == CORNER) {
        face* faces = subcube_colors(subcube);
        bool* rots = subcube_rotations(subcube);

        for (size_t i = 0; i < 3; i++) {
            if (rots[i] == 1) {
                size_t indexA = (i + 1) % 3, indexB = (i + 2) % 3;
                face temp = faces[indexA];
                faces[indexA] = faces[indexB];
                faces[indexB] = temp;
            }
        }
        face result = faces[axis];
        free(faces);
        free(rots);
        return result;
    }
    if (type == SIDE) {
        bool* facesShowing = subcube_rotations(subcube);
        if (facesShowing[axis] != 1) {
            free(facesShowing);
            return BLANK;
        }

        size_t correctFace = 0;
        //if this is the second face stored
        if (axis == OR || (facesShowing[OR] == 0 && axis == BG))
            correctFace = 1;
        if (facesShowing[3] == 1) {
            correctFace = !correctFace;
        }
        free(facesShowing);
        return sideIDtoFaces[subcube & 15][correctFace];

    }

    // if (type == CENTER)
    //     return faceToChar(axis);

    return BLANK;
}

Subcube subcube_default_at(size_t i, size_t j, size_t k)
{
/*
if i/j/k is 0, then val should be W/B/O
if its 2, should be Y/G/R
ignore if 1
*/
    Subcube subcube;
    cubeType type = subcube_type(i, j, k);
    size_t sum;
    switch (type) {
    case CORNER:
        subcube = 0;
        if (i == 2) subcube += 1; // change to yellow if on bottom
        if (j == 2) subcube += 2; // change to green if on back
        if (k == 2) subcube += 4; // change to red if on right
        break;
    case SIDE:
        sum = 9 * i + 3 * j + k;
        // sum will never be 14 (repr. hollow block), so fine to use otherwise
        // has a range of 0-5 and 7-12
        subcube = (sum + 1) / 2 - 1;

        //set up rotations - these formula separate out the sides based on their ID and which face they lack by default
        size_t rot = 5; ///101, lacking
        if (subcube % 3 == 0)
            rot = 3; //011, lacking OR
        else if (subcube > 3 && subcube < 9)
            rot = 6; //110, lacking BG
        subcube |= (rot << 4);
        break;
    default:
        return 0;
    }
    return subcube;
}

enum axis faceToAxis(face face)
{
    return face != BLANK ? face % 3 : -1;
}

Subcube subcube_rotate(Subcube* subcube, cubeType type, rotation rot, face face)
{
    enum axis rotAxis = faceToAxis(face);
    Subcube newSubcube = *subcube;

    if (rot != ROT_180) {
        if (type == CORNER) {
            newSubcube ^= (1 << (rotAxis + 4));
        }
        else if (type == SIDE) {

            size_t mask = 7; //11110000

            // because of how the sides work, need to use the flip bit only when rotated on BG axis
            if (faceToAxis(face) == BG)
                mask = 15; //01110000;
            mask <<= 4;
            mask ^= 1 << (rotAxis + 4);
            newSubcube ^= mask;
        }
    }

    return newSubcube;
}

/**
 * UNIMPLEMENTED
 * Should return the (virtual) faces along the W+, B+, and O+ directions. For example, if the cube were rotated 90 degrees clockwise along the White face,
 *  would return 'wog'.
 * @param subcube the subcube to convert
 * @param i the x-coord
 * @param j the y-coord
 * @param k the z-coord
 * @param result the faces that would be along the primary axes, if they existed
 */

void subcube_primary_faces(const Subcube* subcube, size_t i, size_t j, size_t k, face result[3])
{
    cubeType type = subcube_type(i, j, k);
    size_t pos[3] = { i, j, k };
    // size_t emptySide = 4;
    for (size_t i = 0; i < 3; i++) {
        face color = subcube_color_along_axis(*subcube, i, type);
        // if (color == BLANK)
        //     emptySide = i;

        if (pos[i] == 2)
            color = (color + 3) % 6;
        pos[i] = faceToChar(color);
    }

    if (type == SIDE) {
        //need to generate 3rd side from 2 present
    }
}


char faceToChar(face face)
{
    switch (face) {
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
    default:
        return 'X';
    }
}