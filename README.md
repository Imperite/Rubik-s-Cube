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

A Cube, at its most basic point, requires 20 subcubes to represent it: 12 for the side(edge) cubes, and 8 for the corner cubes.
In the current implementation, a Cube is just a char array with length 20. The first 8 indices are used for indexing the corners, and can be treated as a 2x2x2 array of the corners. Meanwhile, the sides are assigned a specific id and stored in the remaining 12 indices. This allows for all necessary cubes to be stored in a convenient fashion, with the specific indices hidden under the subcube(cube, i, j, k) function that accesses cubes based on their coordiante location instead of the physical location. By hiding under this relative coordinate system, I can easily rotate along any face through a series of constants. During a rotate, the cube automtatically stores its current rotation orientation, allowing it to know it has been rotated.

### Specific Subcube Implementation:

If I store the subcubes in a 3x3x3 matrix, rotations will be more readable (but sacrifice 7 extra subcubes for this - should be viable?)
Each cube would need to encode it's type (could do with id: 26 cubes > 5 bits, or 12 side cubes > 4 bits and 3 bits for corners)
(could also store as the color on specific faces?) - leaves 4 bits to store rotations in
Store rotations in terms of the mirror it reflects from original across: bit 1 is reflection in WY direction, bit 2 in BG, and bit 3 in OR dir
Rotation direction can be done in terms of 3 numbers that determine what cubes are modified / the face used?
Rotation def also gives convenient translation for displaying original subcube

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

# Alternative Algorithms:

- Implement a parallelized form of the brute forcing method - would allow for massive speed-up
- Use hashing to do gradient descent/modified A\*

# Original Goals:

Side note: here are a couple of optimizations I plan to do after the final deadline:

1. Change all rubik's simulation functions to work on the compressed state instead of the struct. this will be even harder to understand, but will minimize the amount of space free/alloced at once (maybe also switch to using only chars, no ints? no need in compressed-only to keep color_to_char or color enums)
2. Similarly, if I switch from a arraylist to a radix tree for storing the different checked states, it should work out much more effieciently in the long run. -- Prob will implement as a multi-nided tree, and either dynamically resize the child pointers or (more likely) just declare a 6-lebgth array?
3. Rewrite the print_cube function; it's pretty repetitive right now, and could use with a little abstraction to make it work
4. Find a way to determine if the route taken to get to this cube is the simplest/compress moves after finding the solution.
5. AFter all that, I'd love to work on making a simple way of plugging in the current state of the cube -- you currently would have to manually encode it in, but I'd love to change that.
6. Finally, I'd love to create a gui for this. so that instead of using text to communicate the solution, I can visually show someone what moves to do.
7. also, Compressing the cube down from a char per side to 3 bytes per side would mean a reduction from 50 bytes per state to 30, so could be worth it if running into space issues (but adds a lot to time cost -- now have to isloate each face for operations, making rotations even harder?)

# Hashing Solver

Working on planning out a solver that works using gradient descent-style optimization. For this to work, I need to 'hash' each cube into a value representing its distance from solved. Then, by seeking out the lowest hash from each rotation, I can (hopefully) create a more efficient solver. I will be very surprised if this works, but it should be possible given the correct hashing function. Ideally, I oculd also combine the hashing with the storage for even more compression, but that's just a pipe dream right now.

## Corner Hashing

The simplest option is to use 3 bits to represent a flip along an axis, as this gives a very easy and light representation, although this has no correlation to distance from starting position, and is identical for any 180 degree rotation.
In general, each subcube can be in one of 24(?) different positions: choose 1 face to be front, and then it has 4 rotations of that position (6\*4). Furthermore, choosing one state as default and then exploring how many moves each other state is from that one, I get:

- 1 permutation 0 steps away
- 6 permutations 1 step away
- 11 permutations 2 steps away
- 6 permutations 3 steps away

Taking the default state as 'wboYGR', these are the distances for each permutation:

- 0: wboYGR
- 1: GwobYR RbwoGY bYoGwR obYRGw wRbYoG woGYRb
- 2: GRwboY GoYbRw RYbowG RwGoYb YGowbR YbRwGo bRYGow bowGRY oYGRwb owbRYG wGRYbo
- 3: GYRbwo RGYobw YRGwob YobwRG bwRGYo oGwRbY

This applies to any cube structure, so theoretically I could hash a cube by transforming each individual subcube into its distance from its origin, sum them up, and get a hash. I'm still not sure that this would fully work, but it would treat the sides and corners the exact same way (which may actually be bad, since there are more sides than corners).

Here is the full breakdown of the moves to get to each face:

- wboYGR:
- woGYRb: White 90
- wRbYoG: White -90
- RbwoGY: Blue 90
- obYRGw: Blue -90
- bYoGwR: Orange 90
- GwobYR: Orange -90
- bowGRY: White 90, Blue 90
- GoYbRw: White 90, Blue -90
- GRwboY: White -90, Blue 90
- bRYGow: White -90, Blue -90
- oYGRwb: White 90, Orange 90
- RwGoYb: White 90, Orange -90
- RYbowG: White -90, Orange 90
- owbRYG: White -90, Orange -90
- wGRYbo: White 90, White 90
- YbRwGo: Blue 90, Blue 90
- YGowbR: Orange 90, Orange 90
- oGwRbY: White 90, White 90, Blue 90
- RGYobw: White 90, White 90, Blue -90
- GYRbwo: White 90, White 90, Orange 90
- bwRGYo: White 90, White 90, Orange -90
- YobwRG: White 90, Blue 90, Blue 90
- YRGwob: White 90, Orange 90, Orange 90

# To Run:

Simply type the command
`gcc main.c ./Cube/cube.c ./Storage/arraylist.c ./Solver/solver.c ./Storage/queue.c -o main`
from the top directory.
