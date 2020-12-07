#include "BOV.h"
#include "inputs.h"

struct ConvexHull{
	int Start;
	int Stop;

	GLfloat *colorDraw;

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
};


void _convexHull_init(struct ConvexHull *myHull, int start, int stop, 
	float coord[][2], int (*hull_function)(int,  float (*)[2], int *), int color);
void _convexHull_update(struct ConvexHull *myHull, int nextPoint, int nextCount);
void _convexHull_free(struct ConvexHull *myHull);

/* Convex Hull Algorithms */
int empty_hull(int nPoints, float coord[][2], int* indexHull);
int* quick_hull(int* S, int size_S, int V_i, int V_j, float coord[][2]);
int jarvis_march(int nPoints, float coord[][2], int* indexHull, bov_window_t* window);
int graham_scan(int nPoints, float coord[][2], int* indexHull);
int chan_(int nPoints, float coord[][2], int* indexHull, int mPoints, bov_window_t* window);

/* Useful Functions */
static void argsort(int nPoints, float coord[][2], int axis, int* argsorted_list);
static float direction(float x1[2],float x2[2],float x3[2]);
int min_dist(int* points, float coord[][2], int I, int J);
int* concat(int* V1, int* V2);

void animate(bov_window_t* window, int nPoints, float coord[][2], int nHull, int* indexHull);

// Display Characteristics
#define POINTS_WIDTH 5e-3
#define POINTS_OUTLINE_WIDTH 5e-5

// Define Some Colors
#define MAXCOLORS 15