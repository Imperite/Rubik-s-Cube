# Rubik's Cube Project

Originally done as a final project for my Data Structures class, with the primary focus on creating something that could theoretically work. Focus currently is on optimizing the data structure used to represent the cube's state, although I eventually would like to program it to solve a cube.

# Data Structure Description:

## Background Information

A Rubik's cube is a simple cube made up of 26 smaller 'subcubes' arranged in a 3x3x3 cube with a hollow center. Within this cube structure, there are 3 different types of subcubes:

- **Center subcubes:** (6 total)
  - These are the unchanging 'anchors' for the cube. They only have 1 face and will never change in relation to each other.
- **Side subcubes:** (12 total)
  - These are the cubes directly adjacent to each of the the faces. They have 2 sides to them, and so are affected by rotations. However, there are only 2 permutations of their rotations, making them relatively simple to track.
- **Corner subcubes:** (8 total)
  - These are the most complicated subcubes, as they make up the corners of the larger cube. They have 3 faces, and so have 3 different permutations of their rotations.

For both the Side and Corner subcubes, a rotation will change at least the position of each cube. Depending on how the underlying data structure is set up, it will likely also require a rotation of some sort for each cube.

## Current Implementation:

In the current data structure, the cube is stored as a series of 48 bytes, with each byte representing one face of a subcube. There are 2 extra bytes; these are used for storing the depth of the cube from the starting position (used in solving). Each face is indexed by position in the primary structure, and stores what its current color is. Then, a set of arrays handles how each face changes with an alteration, allowing the program to just swap faces each time it wants to do a rotate.

# TO-DO:

- Add visualization gui for the solution/exploration process
- Refactor solver to use hash table instead of array
- Change solver to use an algorithm instead of just brute force breadth-first search
  - Use hashes of state to turn solving into gradient descent problem?
- refactor print_Cube to be intuitive and easy to understand
- clean up comments (sheesh, so many!)
- Data Structure Optimizations:
  - Change the depth storage to use one byte (as a number) instead of two (the letter representation)?
  - Change data structure to work off only the rotations of the cubes from the original position - may allow for solving using linear algebra?

# Planning new Data structure:

Store each cube as a transform from it's original position?
To do so, I'd need 6 things: xyz pos and rotation
Can represent rotation using 2 bits (final 2 for type?)
24 orientations for 1 cube (6 faces up, each with 4 rotations for 'front' face)
8 for side cubes

If I store the subcubes in a 3x3x3 matrix, rotations will be more readable (but sacrifice 7 extra subcubes for this - should be viable?)
Each cube would need to encode it's type (could do with id: 26 cubes > 5 bits, or 12 side cubes > 4 bits and 3 bits for corners)
(could also store as the color on specific faces?) - leaves 4 bits to store rotations in
Store rotations in terms of the mirror it reflects from original across: bit 1 is reflection in WY direction, bit 2 in BG, and bit 3 in OR dir
Rotation direction can be done in terms of 3 numbers that determine what cubes are modified / the face used?
Rotation def also gives convenient translation for displaying original subcube
Feasibility:
Takes 1 byte per cube in full 3x3x3, so 27 bytes? > **MUCH BETTER THAN 48**
if store 2 arrays (one of sides and one of corners), could get down to just 20 bytes (excluding 2ish bytes for storing struct in)

# Original Goals:

Side note: here are a couple of optimizations I plan to do after the final deadline:

1. Change all rubik's simulation functions to work on the compressed state instead of the struct. this will be even harder to understand, but will minimize the amount of space free/alloced at once (maybe also switch to using only chars, no ints? no need in compressed-only to keep color_to_char or color enums)
2. Similarly, if I switch from a arraylist to a radix tree for storing the different checked states, it should work out much more effieciently in the long run. -- Prob will implement as a multi-nided tree, and either dynamically resize the child pointers or (more likely) just declare a 6-lebgth array?
3. Rewrite the print_cube function; it's pretty repetitive right now, and could use with a little abstraction to make it work
4. Find a way to determine if the route taken to get to this cube is the simplest/compress moves after finding the solution.
5. AFter all that, I'd love to work on making a simple way of plugging in the current state of the cube -- you currently would have to manually encode it in, but I'd love to change that.
6. Finally, I'd love to create a gui for this. so that instead of using text to communicate the solution, I can visually show someone what moves to do.
7. also, Compressing the cube down from a char per side to 3 bytes per side would mean a reduction from 50 bytes per state to 30, so could be worth it if running into space issues (but adds a lot to time cost -- now have to isloate each face for operations, making rotations even harder?)
