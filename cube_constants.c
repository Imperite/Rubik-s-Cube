// stores index in char array that each face starts
typedef enum faces
{
    BLANK = -1,
    WHITE = 0,
    BLUE = 1,
    ORANGE = 2,
    YELLOW = 3,
    GREEN = 4,
    RED = 5
} face;

typedef enum rotations
{
    NONE = 0,
    ROT_90 = 1,
    ROT_180 = 2,
    ROT_270 = 3
} rotation;

typedef char Cube[27];