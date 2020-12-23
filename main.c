#include "inputs.h"
#include "convex_hull.h"
#include <time.h>

// #include <unistd.h>
#include <stdio.h>
#include<stdlib.h>

int main()
{
	// give a bit of entropy for the seed of rand()
	// or it will always be the same sequence
	int seed = (int) time(NULL);
	srand(seed);

	// we print the seed so you can get the distribution of points back
	printf("seed=%d\n", seed);

#if 1 // put 1 for random polygon
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
	ssize_t read;

	const int nPoints = 18*40;
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
	const int nPoints = 50;
	float (*coord)[2] = malloc(sizeof(coord[0])*nPoints);
	random_points(coord, nPoints);

	/*
	int *indexHull = malloc(sizeof(int)*nPoints);
	// int *indexHullSecond = malloc(sizeof(int)*nPoints);
	//
	// int* S = calloc(nPoints, sizeof(int));
	// int* S_left = calloc(nPoints, sizeof(int));
	// int* S_right = calloc(nPoints, sizeof(int));
	// int left_tracker = 0; int right_tracker = 0;
	//
	//
	// argsort(nPoints, coord, 1, S);
	//
	// for(int i=1; i<nPoints-1; i++){
	// 	if(direction(coord[S[0]], coord[S[nPoints-1]], coord[S[i]])>0){
	// 		S_left[left_tracker] = S[i];
	// 		left_tracker++;
	// 	} else {
	// 		S_right[right_tracker] = S[i];
	// 		right_tracker++;
	// 	}
	// }

	printf("Graham's scan Algorithm --- BEGIN\n");

	clock_t t0 = clock();
	int nHull = graham_scan(nPoints, coord, indexHull);
	// printf("up = %d, down = %d \n", S[0], S[nPoints-1]);
	// int nHull = quick_hull(S_left, left_tracker, S[0], S[nPoints-1], coord, indexHull, 1);
	// int nHullSecond = quick_hull(S_right, right_tracker, S[0], S[nPoints-1], coord, indexHullSecond, 0);
	clock_t t1 = clock();

	// int nHull = sizeof(indexHull)/sizeof(indexHull[0]);
	// int precedent = -1;
	// int* indexHullTotal = calloc(nHull+nHullSecond, sizeof(int));
	// int hull_tracker = 0;
	// for (int i=0; i<nHull-1; i++) {
	// 	if(indexHull[i]!=precedent){
	// 		indexHullTotal[hull_tracker] = indexHull[i];
	// 		hull_tracker++;
	// 	}
	// 	precedent = indexHull[i];
	// }
	// for (int i=nHullSecond-1; i>0; i--) {
	// 	if(indexHullSecond[i]!=precedent){
	// 		indexHullTotal[hull_tracker] = indexHullSecond[i];
	// 		hull_tracker++;
	// 	}
	// 	precedent = indexHullSecond[i];
	// }
	//
	// float (*coordHull)[2] = malloc(sizeof(coordHull[0])*(hull_tracker));
	// for (int i=0; i<hull_tracker; i++) {
	// 	coordHull[i][0] = coord[indexHullTotal[i]][0];
	// 	coordHull[i][1] = coord[indexHullTotal[i]][1];
	// }

	double tm_tot = (double) (t1-t0) / CLOCKS_PER_SEC;

	printf("Jarvis March Algorithm --- END\n");
	printf("  -- Number of points on the grid : N = %d\n", nPoints);
	printf("  -- Number of points in the hull : N = %d\n", nHull);
	printf("  -- Time needed for computations : t = %.3e [s]\n", tm_tot);

	//*
	bov_window_t* window = bov_window_new(800, 800, "Convex Hull Algorithm");
	bov_window_set_color(window, (GLfloat[]){0.9f, 0.85f, 0.8f, 1.0f});

	const GLsizei nPoints_GL = (GLsizei) nPoints;
	GLfloat (*coord_GL)[2] = (GLfloat (*)[2]) coord;

	GLsizei nHull_GL = (GLsizei) nHull;

	bov_points_t *coordDraw = bov_points_new(coord_GL, nPoints_GL, GL_STATIC_DRAW);
	bov_order_t *hullDraw = bov_order_new((GLuint*) indexHull, nHull_GL, GL_STATIC_DRAW);

	bov_points_set_color(coordDraw, (GLfloat[4]) BLACK);
	bov_points_set_outline_color(coordDraw, (GLfloat[4]) {0.3, 0.12, 0.0, 0.25});
	bov_points_set_width(coordDraw, 5E-3);
	bov_points_set_outline_width(coordDraw, 1E-4);

	while(!bov_window_should_close(window)) {
		//bov_line_loop_draw(window, coordDrawHull, 0, nHull);
		bov_line_loop_draw_with_order(window, coordDraw, hullDraw, 0, nHull_GL);
		bov_points_draw(window, coordDraw, 0, nPoints_GL);
		bov_window_update(window);
		//bov_window_update_and_wait_events(window);
	}
	free(indexHull);
	//*/

	printf("Convex Hull Algorithm --- BEGIN\n");

	// struct convex_hull_t *hull = jarvis_march(nPoints, coord, 1);
	// struct convex_hull_t *hull = graham_scan(nPoints, coord, 1);
	// struct convex_hull_t *hull = chan_(nPoints, coord, 1);
	struct convex_hull_t *hull = quickhull(nPoints, coord, 1);

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
