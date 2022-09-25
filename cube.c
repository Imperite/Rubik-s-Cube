#include <stdlib.h>
#include <stdio.h>
#include "cube_constants.c"

// stores index in char array that each face starts
enum faceVal
{
    white = 0,
    blue = 8,
    orange = 16,
    yellow = 24,
    green = 32,
    red = 40
};

// represents the adjacent faces for each specific value; index of each face can be found by face/8
enum faceVal rot_patterns[6][4] = {
    {green, orange, blue, red},
    {yellow, red, white, orange},
    {white, green, yellow, blue},
    {blue, orange, green, red},
    {yellow, orange, white, red},
    {white, blue, yellow, green}};

// creates a basic, solved cube
Cube *init_Cube()
{
    char char_representation[] = {'w', 'b', 'o', 'y', 'g', 'r'};

    Cube *solved = malloc(sizeof(Cube));

    char *cube = calloc(50, sizeof(char));
    for (size_t i = 0; i < 6; i++)
        for (size_t j = 0; j < 8; j++)
            cube[i * 8 + j] = char_representation[i];
    solved->cube = cube;

    solved->depth = 0;
    solved->prev_states = NULL;
    solved->prev_state_length = 0;

    return solved;
}

// destroys cube; not wanting to destroy the state, as that'll be saved elsewhere
void destroy_Cube(Cube *cube)
{
    free(cube->prev_states);
    free(cube);
}

// prints cube out in more graphically applealing manner to help confirm that rotations are done correctly
// not done effieciently, but works
void print_Cube(Cube *cube)
{
    char *chars = (cube->cube);

    printf("\t\t%c %c %c\n", chars[blue], chars[blue + 4], chars[blue + 1]);
    printf("\t\t%c b %c\n", chars[blue + 7], chars[blue + 5]);
    printf("\t\t%c %c %c\n\n", chars[blue + 3], chars[blue + 6], chars[blue + 2]);
    printf("%c %c %c   %c %c %c   %c %c %c\n", chars[orange], chars[orange + 4], chars[orange + 1], chars[white], chars[white + 4], chars[white + 1], chars[red], chars[red + 4], chars[red + 1]);
    printf("%c o %c   %c w %c   %c r %c\n", chars[orange + 7], chars[orange + 5], chars[white + 7], chars[white + 5], chars[red + 7], chars[red + 5]);
    printf("%c %c %c   %c %c %c   %c %c %c\n\n", chars[orange + 3], chars[orange + 6], chars[orange + 2], chars[white + 3], chars[white + 6], chars[white + 2], chars[red + 3], chars[red + 6], chars[red + 2]);
    printf("\t\t%c %c %c\n", chars[green], chars[green + 4], chars[green + 1]);
    printf("\t\t%c g %c\n", chars[green + 7], chars[green + 5]);
    printf("\t\t%c %c %c\n\n", chars[green + 3], chars[green + 6], chars[green + 2]);
    printf("\t\t%c %c %c\n", chars[yellow], chars[yellow + 4], chars[yellow + 1]);
    printf("\t\t%c y %c\n", chars[yellow + 7], chars[yellow + 5]);
    printf("\t\t%c %c %c\n", chars[yellow + 3], chars[yellow + 6], chars[yellow + 2]);
}

char *to_String(Cube *cube)
{
    char *numbers = "0123456789";
    cube->cube[49] = numbers[cube->depth / 10];
    cube->cube[50] = numbers[cube->depth % 10];

    return cube->cube;
}

enum faceVal faceToVal(enum faces face)
{
    switch (face)
    {
    case WHITE:
        return white;
    case BLUE:
        return blue;
    case ORANGE:
        return orange;
    case YELLOW:
        return yellow;
    case GREEN:
        return green;
    case RED:
        return red;
    }
}

Cube *rotate_Cube(Cube *cube, enum faces s, enum rotations rot)
{
    Cube *newCube = init_Cube();

    size_t i, shift, rev_shift;

    for (i = 0; i < 4; i++)
    {
        shift = (rot + i) % 4;
        rev_shift = (rot - i) % 4;

        enum faceVal side = faceToVal(s);

        switch (side)
        {
        case white:
        case yellow:
            // corners
            newCube->cube[side + shift] = cube->cube[side + i];

            // sides
            newCube->cube[side + 4 + shift] = cube->cube[side + i];

            // adj corners
            newCube->cube[rot_patterns[side][shift] + rev_shift] = cube->cube[rot_patterns[side][i] + i];
            newCube->cube[rot_patterns[side][shift] + rev_shift + 1] = cube->cube[rot_patterns[side][i] + i + 1];

            // adj sides
            newCube->cube[rot_patterns[side][shift] + 4 + rev_shift] = cube->cube[rot_patterns[side][i] + 4 + i];
            break;

        case blue:
        case green:
            // corners
            newCube->cube[side + shift] = cube->cube[side + i];

            // sides
            newCube->cube[side + 4 + shift] = cube->cube[side + i];

            // adj corners
            newCube->cube[rot_patterns[side][shift] + (rot_patterns[side][shift] = !yellow ? 0 : 2)] = cube->cube[rot_patterns[side][i] + (rot_patterns[side][i] != yellow ? 0 : 2)];
            newCube->cube[rot_patterns[side][shift] + (rot_patterns[side][shift] = !yellow ? 0 : 2) + 1] = cube->cube[rot_patterns[side][i] + (rot_patterns[side][i] != yellow ? 0 : 2) + 1];

            // adj sides
            newCube->cube[rot_patterns[side][shift] + 4 + (rot_patterns[side][shift] = !yellow ? 0 : 2)] = cube->cube[rot_patterns[side][i] + 4 + (rot_patterns[side][i] != yellow ? 0 : 2)];
            break;

        case orange:
        case red:
            // corners
            newCube->cube[side + shift] = cube->cube[side + i];

            // sides
            newCube->cube[side + 4 + shift] = cube->cube[side + i];

            // adj corners
            newCube->cube[side + i] = cube->cube[side + i];
            newCube->cube[side + i + 1] = cube->cube[side + i + 1];

            // adj sides
            newCube->cube[side + 4 + i] = cube->cube[side + 4 + i];
            break;

        default:
            return NULL;
        }
    }

    return newCube;
}