#include "inputs.h"
#include "convex_hull.h"

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
	// Give a bit of entropy for the seed of rand()
	// or it will always be the same sequence
	int seed = (int) time(NULL);
	srand(seed);

	// We print the seed so you can get the distribution of points back
	printf("seed=%d\n", seed);

#if 1 // Put 1 for polar bear animation
	// Do not forget to put CHAN_ANIMATION to 1 !

	// char buff[1000];
	// getcwd( buff, 1000 );
	// printf("Current working dir: %s\n", buff);

	FILE *file;
	if ((file = fopen("../src/polar_bear.txt", "r"))==NULL) {
		perror("Error while opening the file 'polar_bear.txt'");
		return EXIT_FAILURE;
	}

	char *line = NULL;
	size_t len = 0;
	ssize_t read = getline(&line, &len, file);

	const int nPoints = (int) strtod(line, NULL);
	float (*coord)[2] = malloc(sizeof(coord[0])*nPoints);
	int counter = 0;
	char *point = NULL;

	while ((read = getline(&line, &len, file)) != -1) {
		point = strtok(line, ",");
		coord[counter][0] = strtod(point, NULL);
		point = strtok(NULL, ",");
		coord[counter][1] = strtod(point, NULL);
		counter ++;
	}

	fclose(file);
	if(line)
		free(line);

	struct convex_hull_t *hull = chan_(nPoints, coord, 1);

#else
	const int nPoints = 1000000;
	float (*coord)[2] = malloc(sizeof(coord[0])*nPoints);
	random_points(coord, nPoints);

	printf("Convex Hull Algorithm --- BEGIN\n");
	struct convex_hull_t *hull;

	// hull = jarvis_march(nPoints, coord, 1);
	// hull = graham_scan(nPoints, coord, 1);
	// hull = chan_(nPoints, coord, 1);
	hull = quickhull(nPoints, coord, 1);

	printf("Convex Hull Algorithm --- END\n");
	printf("   →  Method : %s\n", hull->method);
	printf("   →  Number of points on the grid : N = %d\n", hull->nPoints);
	printf("   →  Number of points in the hull : N = %d\n", hull->nHull);
	printf("   →  Time needed for computations : t = %.3e [s]\n", hull->time);
#endif

	bov_window_t* window = bov_window_new(800, 800, "Convex Hull Algorithm");
	bov_window_set_color(window, (GLfloat[]){0.9f, 0.85f, 0.8f, 1.0f});
	convex_hull_display(window, hull);
	bov_window_delete(window);

	free(coord);

	return EXIT_SUCCESS;
}
