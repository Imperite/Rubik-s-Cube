

typedef struct cubeState
{
    // stores all 48 faces, 1 byte per face, and then leaves room to put in depth
    void* cube;

    // used to store all previous states; will have variable length; will store face and rotation for each move, meaning max of 38 chars, but will let specific size be decided at creation.
    char** prev_states;
    // used to store distance from original state
    short depth;
    unsigned short prev_state_length;
} CubeState;