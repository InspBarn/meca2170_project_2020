//#include "inputs.h"
#include "convex_hull.h"
#include <time.h>


int main()
{
	// give a bit of entropy for the seed of rand()
	// or it will always be the same sequence
	int seed = (int) time(NULL);
	srand(seed);

	// we print the seed so you can get the distribution of points back
	printf("seed=%d\n", seed);

	const int nPoints = 500;
	float (*coord)[2] = malloc(sizeof(coord[0])*nPoints);
#if 0 // put 1 for random polygon
	random_polygon(coord, nPoints, 4);
#else
	random_points(coord, nPoints);
#endif

	printf("Convex Hull Algorithm --- BEGIN\n");

	// struct convex_hull_t *hull = jarvis_march(nPoints, coord);
	struct convex_hull_t *hull = chan_(nPoints, coord);

	printf("Convex Hull Algorithm --- END\n");
	printf("   →  Method : %s\n", hull->method);
	printf("   →  Number of points on the grid : N = %d\n", hull->nPoints);
	printf("   →  Number of points in the hull : N = %d\n", hull->nHull);
	printf("   →  Time needed for computations : t = %.3e [s]\n", hull->time);

	bov_window_t* window = bov_window_new(800, 800, "Convex Hull Algorithm");
	bov_window_set_color(window, (GLfloat[]){0.9f, 0.85f, 0.8f, 1.0f});
	convex_hull_display(window, hull);

	free(coord);
	free(hull);
	bov_window_delete(window);

	return EXIT_SUCCESS;
}
