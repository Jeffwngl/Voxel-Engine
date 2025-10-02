#include "rectangle.h"

float rectangleVertices[12] = {
     0.5f,  0.5f, 0.0f,  // top right
     0.5f, -0.5f, 0.0f,  // bottom right
    -0.5f, -0.5f, 0.0f,  // bottom left
    -0.5f,  0.5f, 0.0f   // top left 
};

unsigned int indices[6] = {
    0, 1, 3,  // first Triangle
    1, 2, 3   // second Triangle
};
