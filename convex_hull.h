#include "utils.h"
#include <unistd.h>

#define JARVIS_ANIMATION 1

#define GRAHAM_ANIMATION 0

#define QUICKHULL_ANIMATION 0

#define CHAN_ANIMATION 0
#define CHAN_PRESENTATION_ 0

// Display Time Step Order for Animation
#define TIME_STEP 5e4

/* ----------------------------------------------
	Return a convex_hull_t structure which compute the convex hull
	of the 'nPoints' points set 'coord' according to the Jarvis
	March Algorithm.

	Principle : We start from the leftest point. Then, taking the
	next point if the list, we take the point for which the area
	between them three is the smallest. We then go on while the
	next point is not the leftest one.
---------------------------------------------- */
struct convex_hull_t* jarvis_march(int nPoints,
								   float coord[][2],
								   int display);

/* ----------------------------------------------
	Return a convex_hull_t structure which compute the convex hull
	of the 'nPoints' points set 'coord' according to the Graham
	Scan Algorithm.

	Principle : We want to build 2 convex hulls which join the
	leftest and the rightest points. The first convex hull is going
	upward all the points in 'coord' while the second one is going
	downward. In that respect, we sort all coordinates from the
	leftest to the rightest and going through that sorted array. If
	the hull we obtain by adding the current point then we have to
	remove last points which are inside the final convex hull.
---------------------------------------------- */
struct convex_hull_t* graham_scan(int nPoints,
								  float coord[][2],
								  int display);

/* ----------------------------------------------
	Return a convex_hull_t structure which compute the convex hull
	of the 'nPoints' points set 'coord' according to the Chan
	Algorithm.

	Principle : The set of 'nPoints' points is devided into several
	convex_hull_t structures of 40 points. The convex hull of each
	subsets is computed thanks to the Graham Scan Algorithm. Then
	we perform a Jarvis March between all subsets convex hull to
	compute the global convex hull.
---------------------------------------------- */
struct convex_hull_t* chan_(int nPoints,
							float coord[][2],
							int display);

// CHAN_PRESENTATION cannot be 1 if CHAN_ANIMATION is 0
#if CHAN_ANIMATION
#define CHAN_PRESENTATION CHAN_PRESENTATION_
#else
#define CHAN_PRESENTATION 0
#endif

void jarvis_march_anim(bov_window_t *window, struct convex_hull_t *hull, int end_of);

int quick_hull_rec(int* S, int size_S, int V_i, int V_j, float coord[][2], int* return_hull, int flag_left, struct convex_hull_t* anim_hull, bov_window_t* window);

void quick_hull_anim(bov_window_t *window, struct convex_hull_t *hull, int* actual_hull, int actual_size);

struct convex_hull_t* quickhull(int nPoints, float coord[][2], int display);

void animate(float coord[][2], bov_window_t* window, int* actual_hull, int nHull, int nPoints);


/* ----------------------------------------------
	This function may be used at the very end of the algorithm. It plots
	the set of points in xy coordinates as well as their convex hull.

	By using the left click of the mouse, you can add or remove points
	following :
		→ if the point is not in the set yet, then it is added to the set
		of points and checked if it takes part to the new convex hull.
		→ if it is part of the set but was not in its convex hull, then
		we simply remove the point from the set.
		→ if it is part of the set and its convex hull, then we remove
		the point from the set and compute the new convex hull between
		the points which are around the one that we remove.
---------------------------------------------- */
void convex_hull_display(bov_window_t *window, struct convex_hull_t *hull);
struct convex_hull_t* convex_hull_click_update(struct convex_hull_t *hull, const float point[2]);
