#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "cube_constants.c"


enum axis
{
    WY = 0,
    BG = 1,
    OR = 2
};

typedef enum cubeTypes
{
    CORNER = 0,
    SIDE = 1,
    CENTER = 2,
    HOLLOW = 3
} cubeType;

cubeType subcubeType(size_t i, size_t j, size_t k);

bool* rotations(char subcube, cubeType type);

bool* colors(char subcube);

char defaultCubeAt(size_t i, size_t j, size_t k);

enum faces colorAlongAxis(char subcube, enum axis axis);

char rotateSubcube(char* subcube, cubeType type, rotation rot, face face);


cubeType subcubeType(size_t i, size_t j, size_t k)
{
    size_t numCenter = 0;
    if (i == 1)
        numCenter++;
    if (j == 1)
        numCenter++;
    if (k == 1)
        numCenter++;

    return numCenter;
}


// stores rotations in order OR - BG - WY, with the first value being 1 and the second being 0
bool* rotations(char subcube, cubeType type) {
    bool* rots;
    if (type == CORNER || type == SIDE)
        rots = calloc(3, sizeof(bool));
    else return NULL;

    subcube = subcube >> 4;
    for (size_t i = 0; i < 3; i++)
    {
        rots[i] = (subcube & 1);
    }

    return rots;
}

// stores colors same as rotations: 0 - OR - BG - WY, with first value being 0 and second 1
bool* colors(char subcube) {
    bool* rots;
    rots = calloc(3, sizeof(bool));

    for (size_t i = 0; i < 3; i++)
    {
        rots[i] = (subcube & 1);
    }

    return rots;
}

char defaultCubeAt(size_t i, size_t j, size_t k) {
    /*
    if i/j/k is 0, then val should be W/B/O
    if its 2, should be Y/G/R
    ignore if 1
    */

    char subcube = 0; //default is the WBO combo
    if (i == 2) subcube += 1; // change to yellow if on bottom
    if (j == 2) subcube += 2; // change to green if on back
    if (k == 2) subcube += 4; // change to red if on right


    //can ignore rotations as they are relative to the cube's default, so are all 0
    //TODO: setup default rotations for side subcubes
    return subcube;
}

face colorAlongAxis(char subcube, enum axis axis) {
    bool* cubeColors = colors(subcube);
    switch (axis)
    {
    case WY:
        return (cubeColors[axis] ? WHITE : YELLOW);
    case BG:
        return (cubeColors[axis] ? BLUE : GREEN);
    case OR:
        return (cubeColors[axis] ? ORANGE : RED);
    }
}

enum axis faceToAxis(face face) {
    switch (face)
    {
    case WHITE:
    case YELLOW:
        return WY;
    case BLUE:
    case GREEN:
        return BG;
    case ORANGE:
    case RED:
        return OR;
    }
}

/*
In-place rotation that updates the subcube to match the applied rotation.


The rotation of a corner is stored using three bits, where each bit represents an axis that a cube can be rotated along (WY, BG, OR). So, when rotating, one bit will be flipped each time. A 180 degree rotation is the same as not changing at all, while a reverse 90 and a 90 do the same thing.

he rotation of a side is stored in 4 bits:
    Bit 0: face on WY axis
    Bit 1: face on BG axis
    Bit 2: face on OR axis
    Bit 3: is this pattern flipped from how it normally would be

Bits 0-2 will only have 1 0 in them, as sides will always have one axis they aren't showing anyhting along. Conveniently, they also can't be rotated along that axis, so transformations amount to:
    1) determining the 'preserved' bit
    2) swapping the other 2 bits (can be done using XOR with 1 on them, mask the preserved bit?)
    3) flip bit 3

Since bits 0-2 act as the 'positioning' bits, bit 3 acts as a parity bit that determines the orientation of a side in a specific space. Doing UT'L would result in flipping bit 3, represnting how this flips the OW subcube.
*/
char rotateSubcube(char* subcube, cubeType type, rotation rot, face face) {
    enum axis rotAxis = faceToAxis(face);
    char newSubcube = *subcube;

    if (rot != ROT_180) {
        if (type == CORNER) {
            newSubcube ^= (1 << (rotAxis + 4));
        }
        else if (type == SIDE) {
            size_t mask = 15 << 4; //00001111
            mask ^= 1 << (rotAxis + 4);
            newSubcube ^= mask;
        }
    }

    return newSubcube;
}