#include "BOV.h"
#include "inputs.h"

typedef struct {
	int Start;
	int Stop;

	int nPoints;
	float (*coord)[2];

	int left;
	int *argsorted;

	int nHull;
	int *indexHull;
} ConvexHull;

void _convexHull_init(ConvexHull *myHull, int start, int stop, float coord[][2]);

// void jarvis_march(GLsizei N, GLfloat* x_pos, GLfloat* y_pos, int* hull);
int jarvis_march(int nPoints, float coord[][2], int* hull);
int graham_scan(int nPoints, float coord[][2], int* hull);
int chan_(int nPoints, float coord[][2], int* hull, int mPoints);

static void argsort(int nPoints, float coord[][2], int axis, int* argsorted_list);
static float direction(float x1[2],float x2[2],float x3[2]);
