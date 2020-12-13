#include "BOV.h"
#include "inputs.h"

#include <unistd.h>
#include <time.h>

struct convex_hull_t{
	double time;
	char *method;
	int display;

	int Start;
	int nPoints;
	float (*coord)[2];
	GLsizei nPoints_GL;
	bov_points_t *coordDraw;
	GLfloat *colorDraw;

	int nHull;
	int *hull_idxs;
	GLsizei nHull_GL;
	bov_order_t *hullDraw;
};


void convex_hull_init(struct convex_hull_t *myHull,
					  int start,
					  int stop,
					  float coord[][2],
					  int (*hull_function)(int,  float (*)[2], int *),
					  int color,
					  int display);

void convex_hull_display_init(struct convex_hull_t *hull,
							  int color);

void convex_hull_update(struct convex_hull_t *hull,
						const int *idxs,
						int n);

void convex_hull_partial_update(struct convex_hull_t *hull,
								const int *idxs,
								int start,
								int count,
								int newN);

struct convex_hull_t* convex_hull_click_update(struct convex_hull_t *hull,
											   const float point[2]);

void convex_hull_display(bov_window_t *window,
						 struct convex_hull_t *hull);

// void update_coordinates(struct convex_hull_t *myHull);

/* Convex Hull Algorithms */
int empty_hull(int nPoints, float coord[][2], int* hull_idxs);
int* quick_hull(int* S, int size_S, int V_i, int V_j, float coord[][2]);

struct convex_hull_t* jarvis_march(int nPoints,
								   float coord[][2]);

int graham_scan(int nPoints, float coord[][2], int* hull_idxs);

struct convex_hull_t* chan_(int nPoints,
							float coord[][2]);

/* Useful Functions * /
static void argsort(int nPoints, float coord[][2], int axis, int* argsorted_list);
static float direction(float x1[2],float x2[2],float x3[2]);
int min_dist(int* points, float coord[][2], int I, int J);
int* concat(int* V1, int* V2);
//*/

// Display Time order
#define TIME_STEP 5e4

// Display Characteristics
#define POINTS_WIDTH 5e-3
#define POINTS_OUTLINE_WIDTH 1e-4

// Define Some Colors
#define RGBMAX 255.0
#define MAXCOLORS 15
#define BLACK {0.0/RGBMAX, 0.0/RGBMAX, 0.0/RGBMAX, 1.0}
#define MEDIUMVIOLETRED {199.0/RGBMAX, 21.0/RGBMAX, 133.0/RGBMAX, 1.0}
#define FORESTGREEN {34.0/RGBMAX, 139.0/RGBMAX, 34.0/RGBMAX, 1.0}
#define FIREBRICK {178.0/RGBMAX, 34.0/RGBMAX, 34.0/RGBMAX, 1.0}
#define BLUE {0.0/RGBMAX, 0.0/RGBMAX, 255.0/RGBMAX, 1.0}
#define DARKORANGE {255.0/RGBMAX, 99.0/RGBMAX, 71.0/RGBMAX, 1.0}
#define DEEPPINK {255.0/RGBMAX, 20.0/RGBMAX, 147.0/RGBMAX, 1.0}
#define CHARTREUSE {127.0/RGBMAX, 255.0/RGBMAX, 0.0/RGBMAX, 1.0}
#define CRIMSON {220.0/RGBMAX, 20.0/RGBMAX, 60.0/RGBMAX, 1.0}
#define DEEPSKYBLUE {0.0/RGBMAX, 191.0/RGBMAX, 255.0/RGBMAX, 1.0}
#define GOLD {255.0/RGBMAX, 215.0/RGBMAX, 0.0/RGBMAX, 1.0}
#define HOTPINK {255.0/RGBMAX, 105.0/RGBMAX, 180.0/RGBMAX, 1.0}
#define MEDIUMSPRINGGREEN {0.0/RGBMAX, 250.0/RGBMAX, 154.0/RGBMAX, 1.0}
#define INDIANRED {205.0/RGBMAX, 92.0/RGBMAX, 92.0/RGBMAX, 1.0}
#define STEELBLUE {70.0/RGBMAX, 130.0/RGBMAX, 180.0/RGBMAX, 1.0}
#define PINK {255.0/RGBMAX, 192.0/RGBMAX, 203.0/RGBMAX, 1.0}
#define POINTS_OUTLINE_COLOR {0.0/RGBMAX,76.5/RGBMAX,30.6/RGBMAX, 0.25}
