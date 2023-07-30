#ifndef subcube_h
#define subcube_h

#include "cube.h"
#include <stdbool.h>

/** Types of subcubes */
typedef enum cubeTypes
{
  CORNER = 0,
  SIDE = 1,
  CENTER = 2,
  HOLLOW = 3
} cubeType;

/** Subcube underlying type*/
typedef char Subcube;

/**map from the index of a subcube to its faces*/
extern face sideIDtoFaces[13][2];

/** Null subcube*/
extern const Subcube null;

/**
 * returns a number in the range [0-11] representing a unique index for each side subcube
 * @param i the x-coord
 * @param j the y-coord
 * @param k the z-coord
 * @return size_t  the index of the side subcube in the cube datastructure
 */
size_t subcube_index(size_t i, size_t j, size_t k);

/**
 * Helper method for determining the type of a subcube by its relative position
 * @param i the x-coord
 * @param j the y-coord
 * @param k the z-coord
 * @return cubeType the type of the cube
 */
cubeType subcube_type(size_t i, size_t j, size_t k);

/**
 * stores rotations in order (flippedbit) - OR - BG - WY, with the first value being 1 and the second being 0; flipped bit unused for corners
 * @param subcube the subcube to use
 * @param rots a list to store the rotations(flips) in
 */
void subcube_rotations(const Subcube subcube, bool rots[3]);

/**
 * Colors are stored the same as rotations: 0 - OR - BG - WY, with first value being 0 and second 1. ASSUMES SUBCUBE IS SIDE OR CORNER
 * @param subcube the subcube to use
 * @param type the type of subcube
 * @param colors the array to put the colors in
 */
void subcube_colors(const Subcube subcube, cubeType type, face colors[3]);

/**
 * Returns the face of this subcube along the axis, or NULL if one is not specified.
 *
 * For corners, this is relatively simple:
 * First, extract the 3 colors of of the subcube, along with the rotation bits. Then, for each rotation bit, flip the other two faces if it's true. Finally
 * return the bit at index axis.
 *
 * For sides, it's also really simple.
 *
 * @param subcube the subcube to use
 * @param axis the axis to get the color along
 * @param type the type of the subcube
 * @return face the face of this subcube along the axis
 */
face subcube_color_along_axis(Subcube subcube, enum axis axis, cubeType type);

/**
 * returns the default subcube at the given location
 * @param i the x-coord
 * @param j the y-coord
 * @param k the z-coord
 * @return Subcube the default subcube at this position
 */
Subcube subcube_default_at(size_t i, size_t j, size_t k);

/**
 * Converts the face into the axis the face is along
 * @param face the face to find the axis of
 * @return enum axis the axis the face is along
 */
enum axis faceToAxis(face face);

/**
 * In-place rotation that updates the subcube to match the applied rotation.
 *
 *
 * The rotation of a corner is stored using three bits, where each bit represents an axis that a cube can be rotated along (WY, BG, OR). So, when rotating, one
 *  bit will be flipped each time. A 180 degree rotation is the same as not changing at all, while a reverse 90 and a 90 do the same thing.
 *
 * The rotation of a side is stored in 4 bits:
 *  Bit 0: face on WY axis
 *  Bit 1: face on BG axis
 *  Bit 2: face on OR axis
 *  Bit 3: is this pattern flipped from how it normally would be
 *
 * Bits 0-2 will only have 1 1 in them, as sides will always have one axis they aren't showing anyhting along. Conveniently, they also can't be rotated along
 * that axis, so transformations amount to:
 *  1) determining the 'preserved' bit
 *  2) swapping the other 2 bits (can be done using XOR with 1 on them, mask the preserved bit?)
 *  3) flip bit 3
 *
 * Since bits 0-2 act as the 'positioning' bits, bit 3 acts as a parity bit that determines the orientation of a side in a specific space. Doing UT'L would result in flipping bit 3, represnting how this flips the OW subcube.
 * @param subcube  the subcube to rotate
 * @param type the type of the subcube
 * @param rot  the rotation amount of the subcube
 * @param face the face the subcube is rotating along
 * @return Subcube the new rotated subcube
 */
Subcube subcube_rotate(Subcube* subcube, cubeType type, rotation rot, face face);

/**
 * Converts the face into a char
 * @param face the face to convert
 * @return char the char representation of the face
 */
char faceToChar(face face);


#endif