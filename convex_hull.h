#include "BOV.h"
#include "inputs.h"

// void jarvis_march(GLsizei N, GLfloat* x_pos, GLfloat* y_pos, int* hull);
int jarvis_march(int nPoints, float coord[][2], int* hull);

static int direction(float x1[2],float x2[2],float x3[2]);
