#include "BOV.h"
#include "inputs.h"

typedef struct {
	int Start;
	int Stop;

	int nPoints;
	float (*coord)[2];
	GLsizei nPoints_GL;
	GLfloat (*coord_GL)[2];
	bov_points_t *coordDraw;

	int nHull;
	int *indexHull;
	float (*coordHull)[2];
	GLsizei nHull_GL;
	GLfloat (*coordHull_GL)[2];
	bov_points_t *coordDrawHull;
} ConvexHull;


void _convexHull_init(ConvexHull *myHull, int start, int stop, float coord[][2]);
void _convexHull_initDraw(ConvexHull *myHull, int color);

/* Convex Hull Algorithms */
int* quick_hull(int* S, int size_S, int V_i, int V_j, float coord[][2]);
int jarvis_march(int nPoints, float coord[][2], int* indexHull, bov_window_t* window);
int graham_scan(int nPoints, float coord[][2], int* indexHull);
int chan_(int nPoints, float coord[][2], int* indexHull, int mPoints);

/* Useful Functions */
static void argsort(int nPoints, float coord[][2], int axis, int* argsorted_list);
static float direction(float x1[2],float x2[2],float x3[2]);
int min_dist(int* points, float coord[][2], int I, int J);
int* concat(int* V1, int* V2);

void animate(float coord[][2], bov_window_t* window, int* actual_hull, int nHull, int nPoints);
