// stores index in char array that each face starts
enum faces
{
    WHITE = 0,
    BLUE = 1,
    ORANGE = 2,
    YELLOW = 3,
    GREEN = 4,
    RED = 5
};

enum rotations
{
    NONE = 0,
    ROT_90 = 1,
    ROT_180 = 2,
    ROT_270 = 3
};

typedef struct cube
{
    // stores all 48 faces, 1 byte per face, and then leaves room to put in depth
    char *cube;

    // used to store distance from original state
    short depth;

    // used to store all previous states; will have variable length; will store face and rotation for each move, meaning max of 38 chars, but will let specific size be decided at creation.
    char *prev_states;
    unsigned short prev_state_length;
} Cube;