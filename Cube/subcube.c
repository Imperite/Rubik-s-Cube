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


cubeType subcubeType(size_t i, size_t j, size_t k);

bool* rotations(char subcube);

face* colors(char subcube);

face colorAlongAxis(char subcube, enum axis axis, cubeType type);

char defaultCubeAt(size_t i, size_t j, size_t k);

enum axis faceToAxis(face face);

char rotateSubcube(char* subcube, cubeType type, rotation rot, face face);


cubeType subcubeType(size_t i, size_t j, size_t k)
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

// stores rotations in order (flippedbit) - OR - BG - WY, with the first value being 1 and the second being 0; flipped bit unused for corners
bool* rotations(char subcube) {
    bool rots[4];
    for (size_t i = 0; i < 4; i++)
    {
        rots[i] = 1 & (subcube >> (i + 4));
    }

    return rots;
}

// stores colors same as rotations: 0 - OR - BG - WY, with first value being 0 and second 1. ASSUMES SUBCUBE IS SIDE OR CORNER
face* colors(char subcube) {
    face colors[3];

    // because the value for the axis (and i) aligns with the value for the default colors (WBO),  can just add 3 if that bit is true to shift to the opposite facing color
    for (size_t i = 0; i < 3; i++)
    {
        bool rot = subcube & (1 << i);
        colors[i] = i + (rot * 3);
    }

    return colors;
}

/*
Returns the face of this subcube along the axis, or NULL if one is not specified.

For corners, this is relatively simple:
First, extract the 3 colors of of the subcube, along with the rotation bits. Then, for each rotation bit, flip the other two faces if it's true. Finally, return the bit at index axis.

For sides, it's also really simple
*/
face colorAlongAxis(char subcube, enum axis axis, cubeType type)
{
    // printf("%d %d %d\t", subcube, axis, type);
    if (type == CORNER) {
        face* faces = colors(subcube);
        bool* rots = rotations(subcube);

        for (size_t i = 0; i < 3; i++)
        {
            if (rots[i] == 1) {
                size_t indexA = (i + 1) % 3, indexB = (i + 2) % 3;
                face temp = faces[indexA];
                faces[indexA] = faces[indexB];
                faces[indexB] = temp;
            }
        }

        return faces[axis];
    }
    if (type == SIDE) {
        bool* facesShowing = rotations(subcube);
        if (facesShowing[axis] != 1)
            return BLANK;

        size_t correctFace = 0;
        //if this is the second face stored
        if (axis == OR || (facesShowing[OR] == 0 && axis == BG))
            correctFace = 1;
        if (facesShowing[3] == 1) {
            correctFace = !correctFace;
        }

        return sideIDtoFaces[subcube & 15][correctFace];

    }
    //since the subcube just stores the literal face as its value
    if (type == CENTER && faceToAxis(subcube) == axis)
        return subcube;

    return BLANK;
}

char defaultCubeAt(size_t i, size_t j, size_t k) {
    /*
    if i/j/k is 0, then val should be W/B/O
    if its 2, should be Y/G/R
    ignore if 1
    */

    char subcube;
    cubeType type = subcubeType(i, j, k);
    size_t diff, sum;
    switch (type)
    {
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

        // center ID stored as the face value, so can easily access
    case CENTER:
        diff = i;
        subcube = 0;

        //centers will have 2 1 coords and 1 other coord, so just need to determine which is which
        if (diff == 1)
            if (j == 1) {
                diff = k;
                subcube = 2;
            }
            else {
                diff = j;
                subcube = 1;
            }
        //diff will either be 0 or 2 depending on if it's a main face (WBO) or a alternative face (YGR)
        if (diff == 2)
            subcube += 3;

        break;
    case HOLLOW:
        //done to use unused bit, but can ignore if needed
        subcube = 8;
        break;
    }
    //TODO: setup default rotations (primarily configure flipped bits) for side subcubes
    return subcube;
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

Bits 0-2 will only have 1 1 in them, as sides will always have one axis they aren't showing anyhting along. Conveniently, they also can't be rotated along that axis, so transformations amount to:
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