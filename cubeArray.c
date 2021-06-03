
  enum color {WHITE, BLUE, ORANGE, RED, GREEN, YELLOW};

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