#include "BOV.h"

#include <math.h>
#include <time.h>

/* ---------------------
	Return the distance between x1 and x2
--------------------- */
float distance(const float x1[2], const float x2[2]);

/* ---------------------
	Return the argument of point in coord.
	If it is not in coord, return -1.
--------------------- */
int find(int nPoints, float coord[][2], const float point[2]);

/* ---------------------
	Return the argument of the minimal value along axis.
--------------------- */
int argmin(int nPoints, float coord[][2], int axis);

/* ---------------------
	Return the argument of the maximal value along axis.
--------------------- */
int argmax(int nPoints, float coord[][2], int axis);

/* ---------------------
	Sort coord from its minimal value to its maximal one along axis.
	Store in argsorted_list the arguments of elements such that coord is
	sorted afterwards.
--------------------- */
void argsort(int nPoints, float coord[][2], int axis, int* argsorted_list);

/* ---------------------
	Return the argument of idx in vec.
	If it is not in vec, return -1.
--------------------- */
int argfind(int nPoints, int *vec, int idx);

/* ---------------------
	Return the area of the triangle drawn by x1 → x2 → x3 → x1.
--------------------- */
double direction(float x1[2], float x2[2], float x3[2]);

/* ---------------------
	ANTOINE COMPLETE HERE !!
--------------------- */
struct sort
{
    float value;
    int index;
};
int cmp(const void *a, const void *b);

/*
-------------------------------------------------
                   CONVEX HULL
-------------------------------------------------
*/

struct convex_hull_t{
	double time; // Time needed to compute the current convex hull
	char *method; // Method used to compute the current convex hull
	int display; // Will we plot the points and the convex hull ? 1 if YES else 0

	int Start; // Starting point of coordinates in a bigger coordinates vector (useful in chan algo)
	int nPoints; // Amount of points in the current structure
	float (*coord)[2]; // Coordinates of all points
	GLsizei nPoints_GL;
	bov_points_t *coordDraw;
	GLfloat *colorDraw;

	int nHull; // Amount of points in the current convex hull
	int *hull_idxs; // Indexes of points (in coord[][2]) of points in the current convex hull (sorted !!)
	GLsizei nHull_GL;
	bov_order_t *hullDraw;
};

/* ---------------------
	Initialize a convex_hull_t structure from the coordinates coord[][2].
	After its initialisation, the structure *hull has an empty hull_idxs
	vector but all its points' coordinates have been initialized.
	If display is 1, then we initialize the arguments which will allow us
	to draw the points as well as the convex hull. Else display is 0.
--------------------- */
void convex_hull_init(struct convex_hull_t *hull,
					  int start,
					  int stop,
					  float coord[][2],
					  int display);

/* ---------------------
	Initialize the drawing arguments of the convex_hull_t structure. This
	function is whether called in convex_hull_init if display is 1 or
	afterwards if we decide to show the convex hull later on.
--------------------- */
void convex_hull_display_init(struct convex_hull_t *hull,
							  int color);

/* ---------------------
	Update the hull_idxs vector of the convex_hull_t structure. The integer
	'n' is the amount of element in the new convex hull and 'idxs' is its
	new arguments in the vector of points coordinates of 'hull'.
	It updates the convex hull bov_order_t if necessary (it means if display
	is 1).
--------------------- */
void convex_hull_update(struct convex_hull_t *hull,
						const int *idxs,
						int n);

/* ---------------------
	Partially update the hull_idxs vector of the convex_hull_t structure.
	-→ DOES NOT WORK ACTUALLY !!
--------------------- */
void convex_hull_partial_update(struct convex_hull_t *hull,
								const int *idxs,
								int start,
								int count,
								int newN);


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
