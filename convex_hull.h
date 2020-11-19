#include "BOV.h"
#include "inputs.h"

// void jarvis_march(GLsizei N, GLfloat* x_pos, GLfloat* y_pos, int* hull);
int jarvis_march(int nPoints, float coord[][2], int* hull);
int graham_scan(int nPoints, float coord[][2], int* hull);

static float direction(float x1[2],float x2[2],float x3[2]);

double turn_dir(float x1, float y1, float x2, float y2, float x3, float y3);

