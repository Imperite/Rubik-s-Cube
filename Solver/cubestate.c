#include "../Cube/cube_constants.c"
#include <stdlib.h>
#include <stdio.h>
//Represents an edge leading away from a node in the graph of Cube states, consisting of an side to rotate on and a rotation amount..
typedef struct change {
    size_t change_id;
} Change;

//Represents a path from the starting state, storing the current state and the moves to get from the initial to the current.
typedef struct cubestate {
    Cube cube;
    Change* moves;
    size_t depth;
} CubeState;

//cubestate helper functions

CubeState* cube_state_next(CubeState* start, face side, rotation rot);
void cube_state_destroy(void* c1);
int cube_state_compare(void* c1, void* c2);
int cube_state_compare_with_depth(void* c1, void* c2);
void cube_state_print(void* cs);

void change_print(Change* c);
char* change_array_string(Change changes[], size_t length);



int cube_state_compare(void* c1, void* c2) {
    return cube_compare(&((CubeState*)c1)->cube, &((CubeState*)c2)->cube);
}

int cube_state_compare_with_depth(void* c1, void* c2) {
    int cubeCmp = cube_state_compare(c1, c2);
    if (cubeCmp != 0)
        return cubeCmp;
    return ((CubeState*)c1)->depth >= ((CubeState*)c2)->depth;
}

void cube_state_destroy(void* c1) {
    CubeState* state = ((CubeState*)c1);
    // cube_state_print(state);
    // cube_destroy(state->cube);
    free(state->moves);
    free(state);
}

void cube_state_print(void* cs) {
    CubeState* state = (CubeState*)cs;
    char* changes = change_array_string(state->moves, state->depth);
    char* cubeString = cube_string(&state->cube);
    printf("Depth %zu: %s\n%s\n", state->depth, changes, cubeString);
    free(changes);
    free(cubeString);
    // for (size_t i = 0; i < cube->depth; i++)
    // {
    //     change_print(&cube->moves[i]);
    // }
    // puts("");
    // cube_string(cube->cube);
}

//Gets the CubeState for a cube rotated along face by amount rot.
CubeState* cube_state_next(CubeState* start, face side, rotation rot) {
    CubeState* cs = malloc(sizeof(CubeState));
    cs->depth = start->depth + 1;
    cs->moves = calloc(cs->depth, sizeof(Change));
    for (size_t i = 0; i < start->depth; i++)
    {
        cs->moves[i] = start->moves[i];
    }
    cs->moves[cs->depth - 1] = (Change){
        .change_id = side * 3 + rot - 1
    };
    Cube* rotated = (Cube*)cube_rotate(&start->cube, side, rot);
    for (size_t i = 0; i < 20; i++)
    {
        cs->cube[i] = (*rotated)[i];
    }
    free(rotated);
    return cs;

}

char* face_to_char = "Xwboygr";
char* rot_to_char[] = { "NaN", " 90", "180", "-90" };
char* id_to_str[] = {
    "w 90", "w180", "w-90",
    "b 90", "b180", "b-90",
    "o 90", "o180", "o-90",
    "y 90", "y180", "y-90",
    "g 90", "g180", "g-90",
    "r 90", "r180", "r-90", };
void change_print(Change* c) {
    // printf("%d %d, ", c->face, c->degree);
    // printf("%c%s, ", face_to_char[c->face + 1], rot_to_char[c->degree]);
    printf("%s, ", id_to_str[c->change_id]);
}

char* change_array_string(Change changes[], size_t length) {
    char* result = calloc(length * 6 + 1, sizeof(char));
    for (size_t i = 0; i < length; i++)
    {
        // result[6 * i] = face_to_char[changes[i].face + 1];
        for (size_t j = 0; j < 4; j++)
            result[6 * i + j] = id_to_str[changes[i].change_id][j - 1];
        result[6 * i + 4] = ',';
        result[6 * i + 5] = ' ';
    }
    result[length * 6] = '\0';
    return result;
}