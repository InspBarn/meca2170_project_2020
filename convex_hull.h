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
<<<<<<< HEAD

void argsort(int nPoints, float coord[][2], int axis, int* argsorted_list);

//double turn_dir(float x1, float y1, float x2, float y2, float x3, float y3);
double turn_dir(float x1[2], float x2[2], float x3[2]);

int min_dist(int* points, float coord[][2], int I, int J);

int* concat(int* V1, int* V2);

int* quick_hull(int* S, int size_S, int V_i, int V_j, float coord[][2]);
=======
>>>>>>> 83b3e5fa859bff824257da889aef1d5e108d7d43
