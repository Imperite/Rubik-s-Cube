#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
//Rubik's Cube Solver, written by David Bradley using Repl.it, CodeBlocks with MinGW, and Microsoft Visual Studio. Also drew on pseudocode from wikipedia.

//Side note: here are a couple of optimizations I plan to do after the final deadline: 
/*
    1. Change all rubik's simulation functions to work on the compressed state instead of the struct. this will be even harder to understand, but will minimize the amount of space free/alloced at once (maybe also switch to using only chars, no ints? no need in compressed-only to keep color_to_char or color enums)
    2. Similarly, if I switch from a arraylist to a radix tree for storing the different checked states, it should work out much more effieciently in the long run. -- Prob will implement as a multi-nided tree, and either dynamically resize the child pointers or (more likely) just declare a 6-lebgth array?
    3. Rewrite the print_cube function; it's pretty repetitive right now, and could use with a little abstraction to make it work
    4. Find a way to determine if the route taken to get to this cube is the simplest/compress moves after finding the solution.
    5. AFter all that, I'd love to work on making a simple way of plugging in the current state of the cube -- you currently would have to manually encode it in, but I'd love to change that.
    6. Finally, I'd love to create a gui for this. so that instead of using text to communicate the solution, I can visually show someone what moves to do.
    7. also, Compressing the cube down from a char per side to 3 bytes per side would mean a reduction from 50 bytes per state to 30, so could be worth it if running into space issues (but adds a lot to time cost -- now have to isloate each face for operations, making rotations even harder?)
*/



/*
  Notation:
    XYz
      X = C, S, or (W B O R G Y)
        C = Corner cube
        S = Side cube
        (W B O R G Y) = center cube; one face and constant, so can be used for orientation

      Y = 0-a
        Denotes the index of this cube on the face
      
      z = (w b o r g y) = (0 1 2 3 4 5)?
        Represents the directional face of each cube by the center facing the same direction
    
    zY
      z = (w b o r g y) = (0 1 2 3 4 5)?
        Represents a side of the cube
      Y = 0-7
        Represents what part of that side this block is: 0-3 are 

  Normal Cube state: (original orientation, used in array-struct form)  
    L1: C0 S0 C1    L2: S4 B1 S5    L3: C4 S8 C5
        S1 W0 S2        O2 -- R3        S9 Y5 Sa
        C2 S3 C3        S6 G4 S7        C6 Sb C7

  (NOT USED) Standard method of storing compressed: (just showing cubes, not specific faces)
            Center Cubes                  Side cubes                    
    C0 C1 C2 C3 C4 C5 C6 C7 C8 | S0 S1 S2 S3 S4 S5 S6 S7 S8 S9 Sa Sb | <DEPTH INT>

    Requires storing each cube as a set of 2-3 chars describing faces of cube; would need to factor in rotations in changing cubes, making it more challenging

  Expanded form: (shows faces -- set up for simple rotations?)
      White Corners |  Blue Corners   |  Orange Corners | Yellow Corners  |  Green Corners  |  Red Corners    |         < Group
    C0w C1w C3w C2w | C4b C5b C1b C0b | C4o C0o C2o C6o | C6y C7y C5y C4y | C2g C3g C7g C6g | C1r C5r C7r C3r | . . .   < Original Face
    w0  w1  w2  w3  | b0  b1  b2  b3  | o0  o1  o2  o3  | y0  y1  y2  y3  | g0  g1  g2  g3  | r0  r1  r2  r3  |         < New Terminology

      White Sides   |   Blue Sides    |   Orange Sides  |   Yellow Sides  |   Green Sides   |   Red Sides     |
    S0w S2w S3w S1w | S8b S5b S0b S4b | S4o S1o S6o S9o | Sby Say S8y S9y | S3g S7g Sbg S6g | S5r Sar S7r S2r | <DEPTH INT>
    w4  w5  w6  w7  | b4  b5  b6  b7  | o4  o5  o6  o7  | y4  y5  y6  y7  | g4 g5  g6  g7   | r4  r5  r6  r7  |

  Rotations:
    Needed:
      Must describe completely how to switch values of different positions
        Could use array as in prev version
        Possibility of finding formula?

    white rotates:
      90:
        white corners:
          w0 > w1 > w2 > w3 >
        white sides:
          w4 > w5 > w6 > w7 >
        adjacent corners:
          b2 > r3 > g0 > o1 >
          b3 > r0 > g1 > o2 >
        adjacent sides:
          b6 > r7 > g4 > o5 >

      180:
        white corners:
          w0 > w2 >
          w1 > w3 >
        white sides:
          w4 > w6 >
          w5 > w7 >
        adjacent corners:
          b2 > g0 >
          b3 > g1 >
          r3 > o1 >
          r0 > o2 >
        adjacent sides:
          b6 > g4 >
          o7 > r5 >
        Again, if I store white face side progression, there's a pattern that continues into a 270:
      
      270 (-90)
        white corners:
          w0 > w3 > w2 > w1 >
        white sides:
          w4 > w7 > w6 > w5 >
        adjacent corners:
          b2 > o1 > g0 > r3 >
          b3 > o2 > g1 > r0 >
        adjacent sides:
          b6 > o7 > g4 > r5 >
        index + 3 % 4, or index-1 % 4 works
      Algorithm: Rot white face mod 4 for corners and sides, shift index by 1/-1 for adj corners/sides
      Pattern:  b r g o = 1 5 4 2 = -2 -1
    blue rotates:
      90:
        blue corners:
          b0 > b1 > b2 > b3 >
        blue sides:  
          b4 > b5 > b6 > b7 >
        adjacent corners:
          w0 > o0 > y2 > r0 >
          w1 > o1 > y3 > r1 >
        adjacent sides:
          w4 > o4 > y6 > r4 >
      
      180:
        blue corners:
          b0 > b2 >
          b1 > b3 >
        blue sides:
          b4 > b6 >
          b5 > b7 >
        adjacent corners:
          w0 > y2 >
          w1 > y3 > 
          o0 > r0 >
          o1 > r1 >
        adjacent sides:
          w4 > y6 >
          o4 > r4 >
      
      270 (-90):
        blue corners:
          b0 > b3 > b2 > b1 >
        blue sides:
          b4 > b7 > b6 > b5 >
        adjacent corners:
          w0 > r0 > y2 > o0 >
          w1 > r1 > y3 > o1 >
        adjacent sides:
          w4 > r4 > y6 > o4 >
      
      Diff algorithm (face changes but position remains same, except for in yellow) than white face for adj. rots, same for face rots.
      Pattern: y r w o = 3 5 0 2 = +2 +1
    orange rotates:
      90:
        orange corners:
          o0 > o1 > o2 > o3 >
        orange sides:
          o4 > o5 > o6 > o7 >
        adjacent corners:
          w0 > g0 > y0 > b0 >
          w1 > g1 > y1 > b1 >
        adjacent sides:
          w7 > g7 > y7 > b7 >
      180:
        orange corners:
          o0 > o2 >
          o1 > o3 >
        orange sides:
          o4 > o6 >
          o5 > o7 >
        adjacent corners:
          w0 > y0 >
          g0 > b0 >
        adjacent sides:
          w7 > y7 >
          g7 > b7 >
      270:
        orange corners:
          o0 > o3 > o2 > o1 >
        orange sides:
          o4 > o7 > o6 > o5 >
        adjacent corners:
          w0 > b0 > y0 > g0 >
          w1 > b1 > y1 > g1 >
        adjacent sides:
          w7 > b7 > y7 > g7 >
      Truly simple rotation: All are just incrementing the index as they progress
      Pattern: w g y b = 0 4 3 1 = -2 -1
      
    yellow rotates:
      90:
        yellow corners:
          y0 > y1 > y2 > y3 >
        yellow sides:
          y4 > y5 > y6 > y7 >
        adjacent corners:
          b0 > o3 > g2 > r1 >
          b1 > o0 > g3 > r2 >
        adjacent sides:
          b4 > o7 > g6 > r5 >
      
      180:
        yellow corners:
          y0 > y2 >
          y1 > y3 > 
        yellow sides:
          y4 > y6 >
          y5 > y7 >
        adjacent corners:
          b0 > g2 >
          b1 > g3 >
          o3 > r1 >
          o0 > r2 >
        adjacent sides: 
          b4 > o6 >
          o7 > r5 >
        
      270 (-90):
        yellow corners:
          y0 > y3 > y2 > y1 >
        yellow sides:
          y4 > y7 > y6 > y5 > 
        adjacent corners: 
          b0 > r1 > g2 > o3 >
          b1 > r2 > g3 > o0 >
        adjacent sides:
          b4 > r5 > g6 > o7 >
      same as white, just with different base pattern:
      Pattern: b o g r = 1 2 4 5

  Should work above ^^

  Formula:
    90 degrees means  shift = 1, 180 degrees means shift = 2, 270 degrees means shift = 3 (or -1?)
    White/Yellow:
      store  patttern that represents the order of sides to access w/ lowest value first
        White:  4 2 1 5 = g o b r)
        Yellow: 1 2 4 5 = b o g r
      face corners and sides:
        next_loc = prev_loc + shift % 4
      adj corners:
        next_loc_0 = prev_loc + shift % 4
        next_loc_1 = prev_loc - shift % 4
      adj sides:
        next_loc = prev_loc + shift % 4
    Blue/Green:
      store  patttern that represents the order of sides to access w/ lowest value first
        Blue:   3 5 0 2 = y r w o 
        Green:  3 2 0 5 = y o w r
      face corners and sides:
        next_loc = prev_loc + shift % 4
      adj corners and sides:
        next_loc = (face =! yellow ? prev_loc : prev_loc + 2 )
    Orange/Red:
      store  patttern that represents the order of sides to access w/ lowest value first
        Orange: 0 4 3 1 w g y b
        Red:    0 1 3 4 w b y g
      face corners and sides:
        next_loc = prev_loc + shift % 4
      adj corners:
        next_loc = prev_loc
      adj sides:
        next_loc = prev_loc
*/



//stores index in char array that each face starts
enum faces {WHITE = 0, BLUE = 8, ORANGE = 16, YELLOW = 24, GREEN = 32, RED = 40};
int colors[] = {WHITE, BLUE, ORANGE, YELLOW, GREEN, RED};   //needed to go from index (0-5) to color
char char_representation[] = {'w', 'b', 'o', 'y', 'g', 'r'}; // needed to go from color/index -> char; only really needed for generating cubes
//represents the adjacent faces for each specific value; index of each face can be found by face/8
enum faces rot_patterns[6][4] = {
    {GREEN, ORANGE, BLUE, RED},
    {YELLOW, RED, WHITE, ORANGE},
    {WHITE, GREEN, YELLOW, BLUE},
    {BLUE, ORANGE, GREEN, RED},
    {YELLOW, ORANGE, WHITE, RED},
    {WHITE, BLUE, YELLOW, GREEN}
};

enum rotations {NONE = 0, ROT_90 = 1, ROT_180 = 2, ROT_270 = 3};

typedef struct cube {
  char* cube;        //stores all 48 faces, 1 byte per face, and then leaves room to put in depth
  short depth;          //used to store distance from original state
  char* prev_states;   //used to store all previous states; will have variable length; will store face and rotation for each move, meaning max of 38 chars, but will let specific size be decided at creation.
  size_t prev_state_length;
} Cube;


//generates solved cube state. Not needed, but useful for demostrating how everything's stored and rotations
Cube * init_Cube();
//destroys cube
void destroy_Cube(Cube * cube);
//With the new methods, generating a solved cube is very simple and not costly at all, but randomizing will likely take time.
Cube * randomize_Cube();
//will require more work to visualize effectively, but should be doable
void print_Cube(Cube * cube);
//returns form that will be put into the heap/radix tree to store and compare for later 
char* to_String(Cube * cube);

void rotate_Cube(Cube * cube, enum faces side, enum rotations rot);

//No need for isEqual anymore, as strcmp works just fine










//I have a lot of constant variables here, so I'm using enums to group them up

// both of these are set up in the same orientation(i.e. the front face is white, the top is blue, so the right is red, back is yellow, etc.), and will help make the code more readable.
  //orgainzed so that any face +3 %6 will return the opposite face; helps with generation of cubes
  enum direction {FRONT, BACK, LEFT, RIGHT, TOP, BOTTOM};
  enum color {WHITEC, BLUEC, ORANGEC, REDC, GREENC, YELLOWC};
  enum definitions {NUM_CORNERS = 8, NUM_SIDES = 12, NUM_CENTERS = 6, NUM_CORNER_FACES = 3, NUM_SIDE_FACES = 2, NUM_CENTER_FACES = 1};
  //none is only used in the main solving function to denote if the cube is solved before the 20 move mark.
  //enum rotations {ROT_270 = 0, ROT_180 = 1, ROT_90 = 2, NONE = 3};

  char *numbers = "0123456789";

/*
  this is very abstract -- go read the comment in the Cube_State struct to help understand what each number stored means. Essentially, the number stored descrbes the position inside of the Cube_State where each cube is stored, and each number is positioned in such a way that going to each index stored inside of the Cube_State would make a circle going around that face. This simplifies the process of rotation, and partially the process of generating the first cube.
  The first two arrays stored for each face represent the faces affected by a rotation on this face; the second three arrays represent the faces not affected by that rotation. (one for the corners, and two for the sides -- it was the easiest way to make this work) Also, there's no need to order these specifically, because of how the rotation functions work.
*/
size_t iteration_shifts[6][5][4] = {
  { //front face: -- starts at front left top cube, goes right, down, then left (sid starts at front mid top cube)
    {0, 1, 3, 2}, //corners
    {0, 2, 3, 1},  //sides

    //unchanged faces:
    {4, 5, 6, 7}, //corners
    {4, 5, 6, 7}, //sides
    {8, 9, 10, 11}, //sides
  },
  { //back face: -- same as above, but instead of starting front, starts with the back cube
    {4, 5, 7, 6}, //corners
    {8, 10, 11, 9},  //sides

    //unchanged faces:
    {0, 1, 2, 3}, //corners
    {0, 1, 2, 3}, //sides
    {4, 5, 6, 7}, //sides
  },
  { //left face: -- starts at front left top cube, goes down, back, then up (side starts at front left mid cube)
    {0, 2, 6, 4}, //corners
    {1, 6, 9, 4},  //sides

    //unchanged faces:
    {1, 3, 5, 7}, //corners
    {0, 2, 3, 5}, //sides
    {7, 8, 10, 11}, //sides
  },
  { //right face: -- same as above, but instead of starting left, starts with the right cube
    {1, 3, 7, 5}, //corners
    {2, 7, 10, 5},  //sides

    //unchanged faces:
    {0, 2, 4, 6}, //corners
    {0, 1, 3, 4}, //sides
    {6, 8, 9, 11}, //sides
  },
  { //top face: -- starts at front left top cube, goes right, back, then left (side starts at front mid top cube)
    {0, 1, 5, 4}, //corners
    {0, 5, 8, 4},  //sides

    //unchanged faces:
    {2, 3, 6, 7}, //corners
    {1, 2, 3, 6}, //sides
    {7, 9, 10, 11}, //sides
  },
  { //bottom face: -- same as above, but instead of starting top, starts with the bottom cube
    {2, 3, 7, 6}, //corners
    {3, 7, 11, 6},  //sides

    //unchanged faces:
    {0, 1, 4, 5}, //corners
    {0, 1, 2, 4}, //sides
    {5, 8, 9, 10}, //sides
  },
};
//enum to help make sense of the above array in-code:
enum changed_faces {ROT_CORNERS = 0, ROT_SIDES = 1, SAME_CORNERS = 2, SAME_SIDES_1 = 3, SAME_SIDES_2 = 4};
/*
  this stores how each cube's faces are swapped by a 90 deg rotation (in either direction).
  Use is rotation_swaped_faces[<FACE>][<CORNER=0/SIDE=1>][<FACE INDEX IN ARRAY>]
  The index of each number dictates the original position of that face; the number stored indicates the final position of that face on the cube.
*/
size_t rotation_swaped_faces[6][2][3] = {
  {//front
    {2, 1, 0}, //corners
    {0, 1, 0}  //sides
  },
  {//back
    {2, 1, 0}, //corners
    {0, 1, 0}  //sides
  },
  {//left
    {0, 2, 1}, //corners
    {1, 0, 0}  //sides <- NOTE DIFFERENCE: this inverts them, as top/bottom sides are stored inversely to front/back sides, and so must be switched with every 90 rotation.
  },
  {//right
    {0, 2, 1}, //corners
    {1, 0, 0}  //sides
  },
  {//top
    {1, 0, 2}, //corners
    {0, 1, 0}  //sides
  },
  {//bottom
    {1, 0, 2}, //corners
    {0, 1, 0}  //sides
  }
};

typedef struct change
{
  enum direction face;
  enum rotations degree;
} Change;

//If I deal with the full 3x3 cube in terms of pointers to different individual cubes, this will work out much better for me, as I'll be able to more easily move them around without worrying about mixing it up
/*
  faces[0] = left/right, faces[1] = front/back, faces[2] = top/bottom
  doing it this way allows me to shift in either direction to "rotate" the cube
  */
  typedef struct corner
  {
    enum color faces[NUM_CORNER_FACES];
  } Corner;

  //for the 4 side cubes in the center ring, front_back represents left_right; side represents top_bottom.
  typedef struct sides
  {
    enum color faces[NUM_SIDE_FACES];
  } Side;
  typedef struct center
  {
    enum color face[NUM_CENTER_FACES];
  } Center;

  //Takes up 1,448 bytes per.
  typedef struct cube_state
  {
    //total of 3x3x3-1 =26 different cubes
    Center *centers[NUM_CENTERS];
    Side *sides[NUM_SIDES];
    Corner *corners[NUM_CORNERS];

    //Cube orientation (what each positions represents:
    /*
      front face:
      corner[0]  sides[0]   corner [1]
      sides[1]   center[0]  sides[2]
      corner[2]  sides[3]   corner[3]

      center ring:
      sides[4]   center[1]  sides[5]
      center[2]     -       center[3]
      sides[6]   center[4]  sides[7]

      back face:
      corner[4]  sides[8]   corner [5]
      sides[9]   center[5]  sides[10]
      corner[6]  sides[11]  corner[7]
    */

    Change prev_change;

  } Cube_State;

//implementing using void * so I can reuse for both Side and Corner randomizations
typedef struct ll_node {
  struct ll_node *next;
  void *value;
} ll_Node;

//Rubik's Cube Helper functions functions:
  //initializes each of the subcubes and returns them in a solved cube state
  Cube_State * init_subCubes();

  void destroy_cube(Cube_State **cube);
  //randomly generates a new, randomized cube based on a solved cube state, returns pointer to new randomized cube state
  Cube_State *randomize_cube(Cube_State *solved);
  //checks if the two Cube_States are equal
  bool isEqual(Cube_State *CS1, Cube_State *CS2);
  //compresses cube from 424 bytes down to 6 bytes by converting to string for mass storage This'll help with checking if a new configuration has already been checked.
  char *compress_cube(Cube_State *cube, size_t depth);
  //printing functions:
  void print_cube(Cube_State rubik);
  void print_corner(Corner *c);
  void print_Side(Side *s);
  void print_Center(Center *c);
  void print_Empty(void *v);
  char color_to_char(enum color c);

  //Going to use 6 different functions to define how to change the cube each time; each function will be able to rotate the face
    //rot_amount specifies the amount rotated, in increments of 90: 0 = -90 degrees, 1 = 180 degrees, 2 = 90 degrees
  Cube_State *rotate(Cube_State *previous, enum direction face, size_t rot_amount);

//IF I HAVE TIME: look into changing base storage and rotations to be only string-based, not number-based
//actual Search function:
  //use a tree with 18 children -- generate each of the children, then generate their children?
  //create the first child, then recurse into it and repeat until level 20 has been reached or the cube is solved
  //have pointer to previous node, so that once the solved state is found we can return the moves needed. Each index of the connected_states will store one specific move.
  //could I store previous states to reduce time?
    //to do so:
    //need to compress (shrinks storage space from 1664 bits to 54) -- DONE
    // need a way to compare two rubik's Cube_States/compressed form of two rubik's cube_states -- DONE, use strcmp()
    //need to write an arraylist to store and new vals and check if any old vals exist

    //This'll allow me to store all the states efficiently, letting me do a depth-first search while keeping previous states.

    //store a list of pointers to all created rubik's cube_states

    typedef struct array_list
    {
      size_t size;
      size_t capacity;
      char **prev_states;
    }ArrayList;
    typedef ArrayList * ArrayListPtr;

    //NEEd:
    //create
    ArrayListPtr storage_list_create();
    //destroy
    void storage_list_destroy(ArrayListPtr *listPtr);
    //add
    void storage_list_insert(ArrayListPtr list, char *state);
    //resize
    void storage_list_resize(ArrayListPtr list);
    //contains; will use a binary search to find in list.
    bool storage_list_contains(char **prev_states, size_t end, char *to_find);

  typedef struct tree_node
  {
    //stores all other nodes that are one rotation away from this current Cube_State
    struct tree_node *connected_states[18];
    //stores the index of the previous node pointer inside of the connected_states array
    size_t prev_index;

    Cube_State current;
  } Tree_Node;
  typedef Tree_Node * Tree_NodePtr;

//ACTUAL SOLVING FUNCTION:
void solve(Cube_State *initial_state, Cube_State *solved);
bool solve_cube(Change moves[20], Cube_State *initial_state, Cube_State *solved);

int main()
{
  Cube * solved = init_Cube();
  print_Cube(solved);


  destroy_Cube(solved);
  /*
    //Obviously, I don't have the power to test this function and see if it works fully in 15 min, but here, I've simulated the two main different types of circumstances that could occur, of 3:
      //a) the cube is some number of rotations away on the front face, in which case this the first and second examples prove that it works
      //b) the cube is some number of rotations away on a different face, in which case the third example demostrates that this works
      //c) the cube is not 1 move away, and so should be rotated again. this is proven by the 4th one, which is two different moves away from a solve.
    srand(42);
    Cube_State *solved = init_subCubes();

    //1st EXAMPLE:
    printf("\t");
    Cube_State *rotated = rotate(solved, FRONT, ROT_90);
    //right now, the cube has stored the previous move that it just did, so I need to remove that from it's memory:
    rotated->prev_change.degree = NONE;
    puts("\nTo solve a front 90 rotation:");
    solve(rotated, solved);

    puts("DONE");

    puts("Press Enter to Continue");
    char c;
    scanf("%c", &c);

    //2nd EXAMPLE:
    printf("\n\n\t");
    rotated = rotate(rotated, FRONT, ROT_90);
    rotated->prev_change.degree = NONE;
    puts("\n\nTo solve a cube rotated on front face 180:");
    solve(rotated, solved);


    puts("Press Enter to Continue");
    scanf("%c", &c);

    //3rd EXAMPLE:
    printf("\n\n\t");
    rotated = rotate(solved, BACK, ROT_90);
    rotated->prev_change.degree = NONE;
    puts("\n\nTo solve a cube rotated on Back face 90:");
    solve(rotated, solved);

    puts("Press Enter to Continue");
    scanf("%c", &c);

    //4th EXAMPLE:
    printf("\n\n\t");
    rotated = rotate(solved, FRONT, ROT_90);
    rotated = rotate(rotated, BACK, ROT_180);
    //right now, the cube has stored the previous move that it just did, so I need to remove that from it's memory:
    rotated->prev_change.degree = NONE;
    puts("\n\nTo solve a cube rotated on front face 90 and the back 180:");
    solve(rotated, solved);


    puts("Press Enter to Continue");
    scanf("%c", &c);
    //ARRAYLIST TESTING
    ArrayListPtr storage = storage_list_create();
    size_t iterations = 42;
    size_t length = 2;
    for(size_t i = 0; i < iterations; ++i)
    {
      char *rand_string = malloc(sizeof(char *) * length); //need to get there to be duplicates, so I made it
      for(size_t j = 0; j<length; ++j)
        rand_string[j] = color_to_char(WHITE + (rand() % 6));
        printf("size: %zu\n", storage->size);
      if(i > 0 && !storage_list_contains(storage->prev_states, storage->size, rand_string))
        storage_list_insert(storage, rand_string);
      else if (i == 0)
        storage_list_insert(storage, rand_string);
      else
      {
        printf("FOUND DUPLICATE\n");
        free(rand_string);
      }
    }

    storage_list_destroy(&storage);
  //*/
  return 0;
}

//Solve acts as a wrapper function to print out the results of cube_solve.
void solve(Cube_State *initial_state, Cube_State *solved)
{
  Change *moves = calloc(20, sizeof(Change));

  //first it checks is a FRONT 270 rotation, so the inverse would be a FRON 90 rotation
  bool solve = solve_cube(moves, initial_state, solved);
  printf("solved: %d\n", solve);

  for(size_t i = 0; i < 20 && moves[i].degree != NONE; ++i)
  {
    printf("do a ");
    switch(moves[i].face)
    {
    case FRONT:
        printf("front ");
        break;
    case BACK:
        printf("back ");
        break;
    case LEFT:
        printf("left ");
        break;
    case RIGHT:
        printf("right ");
        break;
    case TOP:
        printf("top ");
        break;
    case BOTTOM:
        printf("bottom ");
        break;
    default:
        printf("ERROR");
    }

    switch(moves[i].degree)
    {
    case ROT_270:
        printf(" 270 degree turn");
        break;
    case ROT_180:
        printf(" 180 degree turn");
        break;
    case ROT_90:
        printf(" 90 degree turn");
        break;
    case NONE:
        printf(" ERROR");
        break;
    }
    puts("");
  }
  free(moves);
}

//ACTUAL SOLVING FUNCTION:
//This has a time complexity of roughly O(n(from writing into arraylist) * n(for performing operations on each state)), assuming n represents the number of different UNIQUE states the cube can be in. without the arraylist, this would instead be just all 18^20 states within 20 moves of the cube.
//time complexity of function is O(n^2)
//Space complexity of function is O(n), for n being the number of unique states. Each new state is stored, and takes up 50 bytes per state (down from 208 originally!)
bool solve_cube(Change moves[20], Cube_State *initial_state, Cube_State *solved)
{
  /*
  plan:
    initialize storage space as static and not null
    use for loops to iterate through each rotation of cube, calling this function on it while not solved and less than 20 calls deep
      problem: how to store previous cube
        Solved: store previous change in each cube_state -- also gives way of telling if this is the original -- it'll be set to null
    we store the moves made so far in the moves array, and just overwrite previous as we elminate various leaves.
    all previous states are compressed down to the least amount of information possible and stored so we can check if we've encountered them before, while also recording the depth they were found at.
  */
  //checked_states keeps track of any states that we've checked before to reduce the number of states to check (so we're not checking duplicate states)
  static ArrayListPtr checked_states = NULL;
  if(checked_states == NULL)
    checked_states = storage_list_create();

  static size_t depth = 0;
  printf("\ndepth: %zu", depth);

  //if this is the first level of the function, check if the original state is solved.
  if(depth == 0)
    if(isEqual(initial_state, solved))
    {
      puts("Cube is already solved!!");
      return true;
    }

  //go through all the possible rotations of the cube
  for(size_t face = FRONT; face <= BOTTOM; ++face)
  {
    for(size_t rot = ROT_270; rot <= ROT_90; ++rot)
    {
      //2-rot is a convenient way to find the inverse rotation -- 90(index 2) goes to 270(index 0), 180(1) goes to 180(1), and 270(0) goes to 90(2) -- basically, this checks if this move would be an inverse of the last
      if(initial_state->prev_change.face == face && initial_state->prev_change.degree == 2-rot)
        continue;

      //find the new move, and record it
      printf("\t");
      moves[depth].face = face;
      moves[depth].degree = rot;
      Cube_State *new = rotate(initial_state, face, rot);
      char *compressed = compress_cube(new, depth);

      //check if this is solved -- if so, we'll reset our static variables and attach a marker signifying we found the end before reaching the max of 20 moves.
      if(isEqual(new, solved))
      {
        //if this is a solved state:
        //set the next move to be null
        if (depth < 19)
            moves[depth+1].degree = NONE;
        puts("Found solution!");
        destroy_cube(&new);
        if(checked_states->size > 0)
            storage_list_destroy(&checked_states);
        return true;
      }

      //otherwise, check if this state has already been found, and explored further along
      if(checked_states->size >= 1 && storage_list_contains(checked_states->prev_states, checked_states->size, compressed))
      {
        printf("\n Already checked, skipping: ");
        puts(compressed);
        free(compressed);
        destroy_cube(&new);
        continue;
      }

      //record this state so we can access it later
      storage_list_insert(checked_states, compressed);

      //check the rotations from this state and see if they would solve it, as long as the depth is less or equal to the max moves of 20.
      //if this isn't the final layer
      if(depth < 20)
      {
        ++depth;
        //if there was a solution found here, return it
        if(solve_cube(moves, new, solved))
        {
          destroy_cube(&new);
          --depth;
          return true;
        }
        --depth;
      }
      destroy_cube(&new);
    }
  }

  return false;
}

//ARRAYLIST FUNCTIONS: most of this will be copied over from the array_list functions we wrote in class, but compare will be of my own design --anything new will be commented
ArrayListPtr storage_list_create()
{
  ArrayListPtr newList = malloc(sizeof(ArrayList));
  newList->size = 0;
  newList->capacity = 8;
  newList->prev_states = calloc(newList->capacity, sizeof(newList->prev_states));
  return newList;
}

//this is new in that I free all the previous found states when destroying the arrayList -- I don't copy them over, which saves a small amount of time
void storage_list_destroy(ArrayListPtr *listPtr)
{
  for(size_t i = 0; i < (*listPtr)->size; ++i)
    free((*listPtr)->prev_states[i]);
  free((*listPtr)->prev_states);
  free(*listPtr);
  *listPtr = NULL;
}

//insertion, sadly, is linear, and that's what slows down the program the most. Implementing this as a hash would have been better, but I don't have the time for that right now
void storage_list_insert(ArrayListPtr list, char *state)
{
  storage_list_resize(list);
  size_t i;

  for(i = list->size++; i > 0 && strcmp((state+2), (list->prev_states[i-1]+2)) < 0; --i)
    list->prev_states[i] = list->prev_states[i-1];

  list->prev_states[i] = state;
  printf("\n\tAdding ");
  puts(state);
}

void storage_list_resize(ArrayListPtr list)
{
  if(list->size == list->capacity)
  {
    puts("");
    list->capacity *=2;
    list->prev_states = realloc(list->prev_states, list->capacity * sizeof(char *));
  }
}

//BASED ON PSEUDOCODE FROM WIKIPEDIA; was having trouble getting it to work recursively based on the code I made in class, so gave up and looked up how to do it on wikipedia.
//storage_list_contains() a binary search to find out if the current state was already found, and then returns true if we found this state through a longer path than before.
bool storage_list_contains(char **prev_states, size_t size, char *to_find)
{
    puts("");
    int left = 0;
    int right = size-1;
    int mid;
    while(left <= right)
    {
        mid = (left + right)/2;
        int compare = strcmp((to_find+2), (prev_states[mid]+2));
        if(compare == 1)
            left = mid + 1;
        else if(compare == -1)
            right = mid-1;
        else if(compare == 0 && prev_states[mid][0] * 10 + prev_states[mid][1] < to_find[0] * 10 + to_find[1])
            return true;
    }
    return false;
}




//RUBIK's CUBE CHAR ARRAY SIMULATION FUNCTIONS:
//creates a basic, solved cube
Cube * init_Cube() {
  Cube * solved = malloc(sizeof(Cube));
  char* cube = calloc(50, sizeof(char));
  for(size_t i = 0; i < 6; i++)
    for (size_t j = 0; j < 8; j++)
      cube[i*8 + j] = char_representation[i];
  solved->cube = cube;

  solved->depth = 0;
  solved->prev_states = NULL;
  solved->prev_state_length = 0;
  
  return solved;
}

//destroys cube; not wanting to destroy the state, as that'll be saved elsewhere
void destroy_Cube(Cube * cube)
{
  free(cube->prev_states);
  free(cube);
}

//prints cube out in more graphically applealing manner to help confirm that rotations are done correctly
//not done effieciently, but works
void print_Cube(Cube * cube)
{
  char* chars = (cube->cube);
  
  printf("\t\t%c %c %c\n",  chars[BLUE], chars[BLUE+4], chars[BLUE+1]);
  printf("\t\t%c b %c\n",   chars[BLUE+7], chars[BLUE+5]);
  printf("\t\t%c %c %c\n\n",  chars[BLUE+3], chars[BLUE+6], chars[BLUE+2]);
  printf("%c %c %c   %c %c %c   %c %c %c\n",  chars[ORANGE], chars[ORANGE+4], chars[ORANGE+1], chars[WHITE],    chars[WHITE+4], chars[WHITE+1], chars[RED], chars[RED+4], chars[RED+1]);
  printf("%c o %c   %c w %c   %c r %c\n",   chars[ORANGE+7], chars[ORANGE+5], chars[WHITE+7], chars[WHITE+5], chars[RED+7], chars[RED+5]);
  printf("%c %c %c   %c %c %c   %c %c %c\n\n",  chars[ORANGE+3], chars[ORANGE+6], chars[ORANGE+2], chars[WHITE+3], chars[WHITE+6], chars[WHITE+2], chars[RED+3], chars[RED+6], chars[RED+2]);
  printf("\t\t%c %c %c\n", chars[GREEN], chars[GREEN+4], chars[GREEN+1]);
  printf("\t\t%c g %c\n", chars[GREEN+7], chars[GREEN+5]);
  printf("\t\t%c %c %c\n\n", chars[GREEN+3],chars[GREEN+6],chars[GREEN+2]);
  printf("\t\t%c %c %c\n", chars[YELLOW], chars[YELLOW+4], chars[YELLOW+1]);
  printf("\t\t%c y %c\n", chars[YELLOW+7], chars[YELLOW+5]);
  printf("\t\t%c %c %c\n", chars[YELLOW+3], chars[YELLOW+6], chars[YELLOW+2]);
  
}

char * to_String(Cube *cube)
{
  char* numbers = "0123456789";
  cube->cube[49] = numbers[cube->depth/10];
  cube->cube[50] = numbers[cube->depth%10];

  return cube->cube;
}

//RUBIK'S CUBE Struct/Array SIMULATION FUNCTIONS:
//init_subcubes creates a basic, solved rubik's cube that can then be modified how you want. This is currently stored in a struct; in the future, I plan to work on making this a string instead, although that will make it even harder to understand.
Cube_State * init_subCubes()
{
  puts("Generating new, solved Cube");
  Cube_State *solved = malloc(sizeof(Cube_State));

  //initalize all the subcubes to exist
  size_t index;
  for(index = 0; index < NUM_CORNERS; ++index)
    solved->corners[index] = malloc(sizeof(Corner));


  for(index = 0; index < NUM_SIDES; ++index)
    solved->sides[index] = malloc(sizeof(Side));

  for(index = 0; index < NUM_CENTERS; ++index)
    solved->centers[index] = malloc(sizeof(Center));

  //sets centers to be the correct color: 0 1 2 3 4 5 to w b o r y g
  for(index = 0; index < NUM_CENTERS; ++index)
    solved->centers[index]->face[0] = (WHITEC + index);


  for(index = 0; index < 4; ++index)
  {
    //WHILE READING THIS:
    //I'd highly recommend having the "cube orientation comment" in the cube_state struct pulled up to help visualize, and (ideally) a rubik's cube, to help visualize what's going on. Each pair of faces was originally made in its own for loop, then brought together once they had be tested.
    //also, this has gotten simpler while writing the program, but I've left comments to show what it used to be. The recommendation above still stands, but is less conceptually necessary than it was before.
      //Now, I use the fact that I can select the correct subcubes based on which would be changed when you rotate a cube in order to set the faces of the correct cubes to the right colors.

    //front and back
      //very simple, just needs 0 1 2 3 for corners and sides (and inverse for back)
      //used to use index and NUM_CORNERS-index-1 here, but changed for consistency
    solved->corners[iteration_shifts[FRONT][ROT_CORNERS][index]]->faces[1] = WHITEC;
    solved->corners[iteration_shifts[BACK][ROT_CORNERS][index]]->faces[1] = YELLOWC;

    solved->sides[iteration_shifts[FRONT][ROT_SIDES][index]]->faces[0] = WHITEC;
    solved->sides[iteration_shifts[BACK][ROT_SIDES][index]]->faces[0] = YELLOWC;

    //left and right
      //left corners need 0 2 4 6 and shifted 1 to the right for the right side
      //used to use index*2 and index*2+1 here
    solved->corners[iteration_shifts[LEFT][ROT_CORNERS][index]]->faces[0] = ORANGEC;
    solved->corners[iteration_shifts[RIGHT][ROT_CORNERS][index]]->faces[0] = REDC;

    //this is very complicated. I ran into a problem of how to designate which face was "side" and which was "front_back" for sides[4] thru sides[7], and so this is the simplest way I thought of to handle it. In this case, I set the "front_back" face to be on the left_right (x) axis, if necessary, and work from there. This if statement filters out the sides for which this is necessary. Then, I use the equation to get the sequence 1 4 9 6 (the indices of the side cubes for the left face), and set those to be the correct face, as well as shift one over and set red face.
      //used to use (index+1) * (index+1) % 10 for the left face and (index+1) * (index+1) % 10 +1 for the right
    if(index%2 == 0)
    {
      solved->sides[iteration_shifts[LEFT][ROT_SIDES][index]]->faces[1] = ORANGEC;
      solved->sides[iteration_shifts[RIGHT][ROT_SIDES][index]]->faces[1] = REDC;
    }
    else
    {
      solved->sides[iteration_shifts[LEFT][ROT_SIDES][index]]->faces[0] = ORANGEC;
      solved->sides[iteration_shifts[RIGHT][ROT_SIDES][index]]->faces[0] = REDC;
    }

    //top and bottom
      //for corners, need 0 1 4 5 in some order, using order 4 5 0 1 here
      //used to use (4+index) % 6 for the top and (4+index) % 6 + 2 for the bottom
    solved->corners[iteration_shifts[TOP][ROT_CORNERS][index]]->faces[2] = BLUEC;
    solved->corners[iteration_shifts[BOTTOM][ROT_CORNERS][index]]->faces[2] = GREENC;

    //sides need 0 4 5 8 and 3 6 7 11, using 4 5 8 0 and 6 7 11 3 here
        //4 * (index %2) + (index/2)%2 * 4 + ((index+1)/3) %2 - ((index)/3 %2) works for top, but is unnecessarily complicated and doesn't work for bottom
        //used to use an if statemnt for if index is less than 2, and use 4+index (or 6+index) if true, or (6+index) % 9 (or (6+index) % 9 + 3) if false
    solved->sides[iteration_shifts[TOP][ROT_SIDES][index]]->faces[1] = BLUEC;
    solved->sides[iteration_shifts[BOTTOM][ROT_SIDES][index]]->faces[1] = GREENC;
  }
  solved->prev_change.degree = NONE;
  solved->prev_change.face = FRONT;
  puts("Done");
  return solved;
}

//this frees the entire cube, so that nothing is left over allocated -- this allows me to safely run this program without using arraylist to store previous states, as the program will remain at a constant size.
void destroy_cube(Cube_State **cubePtr)
{
  //printf("Destroying cube %p", *cubePtr);
  puts("");
  Cube_State *cube = *cubePtr;
  //using same logic as in rotate to parse through entire cube in 4 iterations
  for(size_t i = 0; i < 4; ++i)
  {
    //free corners
    free(cube->corners[i]);
    free(cube->corners[i+4]);

    //free sides
    free(cube->sides[i]);
    free(cube->sides[i+4]);
    free(cube->sides[i+8]);

    //free centers
    if(i < 3)
    {
      free(cube->centers[i]);
      free(cube->centers[i+3]);
    }
  }
  //free actual Cube_State:
  free(cube);
  *cubePtr = NULL;
}

//this, as the name suggests, fully randomizes the cube, in both the position and orientation of each of the subcubes.
Cube_State *randomize_cube(Cube_State *solved)
{
  puts("Generating new randomized Cube:");
  Cube_State *randomized = malloc(sizeof(Cube_State));

  //adding centers
  puts("  Adding Centers");
  size_t i = 0, nextPos;
  for(; i < NUM_CENTERS; ++i)
  {
    randomized->centers[i] = malloc(sizeof(Center));
    randomized->centers[i]->face[0] = WHITEC + i;
  }

  //adding corners
  puts("  Creating linked list for adding corners");
  ll_Node *corners = NULL, *last_corner = NULL;
  for(i = 0; i < NUM_CORNERS; ++i)
  {
    ll_Node *next_corner = malloc(sizeof(ll_Node));
    next_corner->value = solved->corners[i];


    randomized->corners[i] = NULL;

    if(last_corner == NULL)
      corners = next_corner;
    else
      last_corner->next = next_corner;
    last_corner = next_corner;
  }

  puts("  Adding Corners to new cube");
  //using quadratic probing to add to new list;
  while(corners != NULL)
  {
    for(nextPos = rand() % NUM_CORNERS, i = 0; randomized->corners[nextPos] != NULL; nextPos = (nextPos+i*i) % NUM_CORNERS, ++i);
    randomized->corners[nextPos] = malloc(sizeof(Corner *));

    //also randomly rotates the cube:
    size_t shift = rand() % NUM_CORNER_FACES;
    randomized->corners[nextPos]->faces[(0+shift) % 3] = ((Corner *)(corners->value))->faces[0];
    randomized->corners[nextPos]->faces[(1+shift) % 3] = ((Corner *)(corners->value))->faces[1];
    randomized->corners[nextPos]->faces[(2+shift) % 3] = ((Corner *)(corners->value))->faces[2];

    ll_Node *temp = corners;
    corners = corners->next;
    free(temp);
  }

  //adding sides
  puts("  Creating Linked List for sides");
  ll_Node *sides = NULL, *last_side = NULL;
  for(i = 0; i < NUM_SIDES; ++i)
  {
    ll_Node *next_side = malloc(sizeof(ll_Node));
    next_side->value = solved->sides[i];

    randomized->sides[i] = NULL;

    if(last_side == NULL)
      sides = next_side;
    else
      last_side->next = next_side;
    last_side = next_side;
  }

  puts("  Finished. Adding Sides to Cube");
  while(sides != NULL)
  {
    for(nextPos = rand() % NUM_SIDES, i = 0; randomized->sides[nextPos] != NULL; nextPos = (nextPos+i*i) % NUM_SIDES, ++i);

    randomized->sides[nextPos] = malloc(sizeof(Side *));
    //also randomly flips the cube:
    size_t shift = rand() % NUM_SIDE_FACES;
    randomized->sides[nextPos]->faces[(0+shift)%2] = ((Side *)(sides->value))->faces[0];
    randomized->sides[nextPos]->faces[(0+shift)%2] = ((Side *)(sides->value))->faces[1];

    ll_Node *temp = sides;
    sides = sides->next;
    free(temp);
  }

  puts("Exiting Cube Randomization\n");
  return randomized;
}

//this is a simple comparison; name says it all about this.
bool isEqual(Cube_State *CS1, Cube_State *CS2)
{
  size_t i,j;
  for(i = 0; i < NUM_CORNERS; ++i)
    for(j = 0; j < NUM_CORNER_FACES; ++j)
      if(CS1->corners[i]->faces[j] != CS2->corners[i]->faces[j])
        return false;

  for(i = 0; i < NUM_SIDES; ++i)
    for(j = 0; j < NUM_SIDE_FACES; ++j)
      if(CS1->sides[i]->faces[j] != CS2->sides[i]->faces[j])
        return false;

  //centers can never change their position, and so there's no need to check them

  return true;
}

//compress cube takes a struct datatype and turns it into a char array, reducing space per from 208 to 8 bytes.
char *compress_cube(Cube_State *cube, size_t depth)
{
  //need space for each face of each cube, except for the center faces, as well as depth of cube
  char *compressed_cube = calloc(NUM_CORNERS * 3 + NUM_SIDES * 2 + 2, sizeof(char));
  //storing depth at beginning so I can skip them when comparing

  //both digits of depth can't be 0, so this is done instead
  compressed_cube[0] = numbers[depth/10];
  compressed_cube[1] = numbers[depth % 10];

  size_t index, face;
  for(index = 0; index < NUM_CORNERS * 3; index += 3)
    for(face = 0; face < 3; ++face)
      //I have to use a little arithemetic to find the index in cube->corners that I'm pulling from, and then use the color_to_char() function to convert the number into a legible character
      compressed_cube[2+index+face] = color_to_char(cube->corners[index/3]->faces[face]);

  for(; index < NUM_CORNERS*3 + NUM_SIDES * 2; index += 2)
    for(face = 0; face < 2; ++face)
      compressed_cube[2+index+face] = color_to_char(cube->sides[(index - NUM_CORNERS*3)/2]->faces[face]);
  return compressed_cube;
}

//print cube prints out each cube in levels: it prints out the subcubes in the front, then those in the middle, then those in the back.
//TO-DO: inefficient, but works -- could use splitting into a submethod to simplify (which'll require 6 inputs, so is it really simplifying?
void print_cube(Cube_State rubik)
{
  //general idea: print out front layer cubes, then center layer cubes, then back layer cubes
  //for firsst layer: print out first corner, first side, second corner, \n, second side, first center, third side, \n, third corner, fourth side, fourth corner

  size_t corner_index = 0;
  size_t side_index = 0;
  size_t center_index = 0;

  //should be able to turn into method? -- pass in array for corner, side, and center, and functions for all? (set center to null for mid ring)
  //front cubes:
  puts("Cube State:");
  print_corner(rubik.corners[corner_index++]);
  print_Side(rubik.sides[side_index++]);
  print_corner(rubik.corners[corner_index++]);
  puts("");
  print_Side(rubik.sides[side_index++]);
  print_Center(rubik.centers[center_index++]);
  print_Side(rubik.sides[side_index++]);
  puts("");
  print_corner(rubik.corners[corner_index++]);
  print_Side(rubik.sides[side_index++]);
  print_corner(rubik.corners[corner_index++]);
  puts("\n");

  //middle cubes:
  print_Side(rubik.sides[side_index++]);
  print_Center(rubik.centers[center_index++]);
  print_Side(rubik.sides[side_index++]);
  puts("");
  print_Center(rubik.centers[center_index++]);
  print_Empty(rubik.centers[center_index]); //must pass in something, not used so doesn't matter
  print_Center(rubik.centers[center_index++]);
  puts("");
  print_Side(rubik.sides[side_index++]);
  print_Center(rubik.centers[center_index++]);
  print_Side(rubik.sides[side_index++]);
  puts("\n");

  //back cubes:
  print_corner(rubik.corners[corner_index++]);
  print_Side(rubik.sides[side_index++]);
  print_corner(rubik.corners[corner_index++]);
  puts("");
  print_Side(rubik.sides[side_index++]);
  print_Center(rubik.centers[center_index++]);
  print_Side(rubik.sides[side_index++]);
  puts("");
  print_corner(rubik.corners[corner_index++]);
  print_Side(rubik.sides[side_index++]);
  print_corner(rubik.corners[corner_index++]);
  puts("\nEnd Cube State\n");

}

void print_cube_slice(void * corner_Arr, size_t corner_index, void * side_Arr, size_t size_index, char center)
{

}

//PRINT HELPER FUNCTIONS FINISHED
  //extra spaces in print_Side and print_Center help align all the cubes so they have a neat pattern

  //this prints out the three faces of a subcube: <left/right>, <front/back>, <top/bottom>
  void print_corner(Corner *c)
  {
    printf("%c<-%c->%c\t", color_to_char(c->faces[0]), color_to_char(c->faces[1]), color_to_char(c->faces[2]));
  }

  void print_Side(Side *s)
  {
    printf(" %c->%c \t", color_to_char(s->faces[0]), color_to_char(s->faces[1]));
  }

  void print_Center(Center *c)
  {
    printf("  %c   \t", color_to_char(c->face[0]));
  }

  void print_Empty(void *v)
  {
    printf("  -   \t");
  }

  //used to change from a size_t to the char.
  char color_to_char(enum color c)
  {
    switch(c)
    {
      case WHITEC:
        return 'w';
      case BLUEC:
        return 'b';
      case REDC:
        return 'r';
      case YELLOWC:
        return 'y';
      case GREENC:
        return 'g';
      case ORANGEC:
        return 'o';
      default:
        return 'z';
    }
  }

  //rotate is complicated, like init_subcubes -- originally, I used an equation to determine which faces and in what order to rotate, but this got overly complex and confusing, so I just created an array to store the different rotations and how they affected the cube overall for me.
Cube_State *rotate(Cube_State *previous, enum direction face, size_t rot_amount)
{
  //plan:
  //gen new cube
  printf("rotated %d clockwise on face %d: ", (rot_amount == 0 ? 270 : (rot_amount == 1 ? 180 : 90)), face);
  Cube_State *rotated = malloc(sizeof(Cube_State));

  rotated->prev_change.degree = rot_amount;
  rotated->prev_change.face = face;

  //copy over all prev data from old cube, rotating as you go
  size_t i, j, target_index;

  //try putting the pointers in an array to rotate around?
  //need to transfer from in-order to stored order (and reverse): -- starts from top right corner
  //0 1 2 3  <in-order/array order>
  //0 1 3 2  <stored order -- how the values are stored inside of cube_state>
    // i+i/2-2*(i/3) works both ways

  Corner *front_corners[4];
  Side *front_sides[4];
  for(i = 0; i <4; ++i)
  {
    front_corners[i] = previous->corners[iteration_shifts[face][ROT_CORNERS][i]]; //previously used i+i/2-2*(i/3)

    front_sides[i] = previous->sides[iteration_shifts[face][ROT_SIDES][i]]; //previously used (i + i - i/2) % 4
  }

  for(i=0; i<4; ++i)
  {
  //Adding rotated corners:
    //finds the current place we are inserting to
    target_index = iteration_shifts[face][ROT_CORNERS][i]; //previously used i+i/2-2*(i/3)
    rotated->corners[target_index] = malloc(sizeof(Corner));
    for(j = 0; j < NUM_CORNER_FACES; ++j)
      //if statement is needed because the "left_right" and "top_bottom" faces switch when rotated 90 deg in either direction, but not 180 deg.
      if(rot_amount != ROT_180)
        rotated->corners[target_index]->faces[rotation_swaped_faces[face][0][j]] = front_corners[(i+rot_amount+1) %4]->faces[j]; //adding one here means it will always rotate some
      else
        rotated->corners[target_index]->faces[j] = front_corners[(i+rot_amount+1) %4]->faces[j];

  //Adding rotated sides:
    target_index = iteration_shifts[face][ROT_SIDES][i]; //previously used (i + i - i/2) % 4
    rotated->sides[target_index] = malloc(sizeof(Side));
    for(j = 0; j < NUM_SIDE_FACES; ++j)
      if(rot_amount != ROT_180)
        rotated->sides[target_index]->faces[rotation_swaped_faces[face][1][j]] = front_sides[(i+rot_amount+1) %4]->faces[j];
      else
        rotated->sides[target_index]->faces[j] = front_sides[(i+rot_amount+1) %4]->faces[j];

  //Adding same corners:
    rotated->corners[iteration_shifts[face][SAME_CORNERS][i]] = malloc(sizeof(Corner));
      for(j = 0; j < 3; ++j)
        rotated->corners[iteration_shifts[face][SAME_CORNERS][i]]->faces[j] = previous->corners[iteration_shifts[face][SAME_CORNERS][i]]->faces[j];

  //Adding same sides:
    rotated->sides[iteration_shifts[face][SAME_SIDES_1][i]] = malloc(sizeof(Side));
    rotated->sides[iteration_shifts[face][SAME_SIDES_2][i]] = malloc(sizeof(Side));
      for(j = 0; j < 2; ++j)
      {
        rotated->sides[iteration_shifts[face][SAME_SIDES_1][i]]->faces[j] = previous->sides[iteration_shifts[face][SAME_SIDES_1][i]]->faces[j];
        rotated->sides[iteration_shifts[face][SAME_SIDES_2][i]]->faces[j] = previous->sides[iteration_shifts[face][SAME_SIDES_2][i]]->faces[j];
      }

  //Adding centers:
    //because centers is not a multiple of 4, I loop through it 3 times instead, working from both the front and end of the array.
    if(i < NUM_CENTERS/2)
    {
      rotated->centers[i] = malloc(sizeof(Center));
      *rotated->centers[i]->face = WHITEC + i;

      rotated->centers[NUM_CENTERS-i-1] = malloc(sizeof(Center));
      *rotated->centers[NUM_CENTERS-i-1]->face = YELLOWC - i;
    }
  }

  //return new cube_state
  return rotated;
}
