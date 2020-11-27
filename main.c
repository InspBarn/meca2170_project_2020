//#include "inputs.h"
#include "convex_hull.h"
#include <time.h>

/*
	int main()
	{
		// give a bit of entropy for the seed of rand()
		// or it will always be the same sequence
		int seed = (int) time(NULL);
		srand(seed);

		// we print the seed so you can get the distribution of points back
		printf("seed=%d\n", seed);

		bov_window_t* window = bov_window_new(800, 800, "My first BOV program");
		bov_window_set_color(window, (GLfloat[]){0.9f, 0.85f, 0.8f, 1.0f});

		const GLsizei nPoints = 10;
		GLfloat (*coord)[2] = malloc(sizeof(coord[0])*nPoints);
	#if 0 // put 1 for random polygon
		random_polygon(coord, nPoints, 4);
	#else
		random_points(coord, nPoints);
	#endif

		bov_points_t *coordDraw = bov_points_new(coord, nPoints, GL_STATIC_DRAW);
		bov_points_set_color(coordDraw, (GLfloat[4]) {0.0, 0.0, 0.0, 1.0});
		bov_points_set_outline_color(coordDraw, (GLfloat[4]) {0.3, 0.12, 0.0, 0.25});
		GLint i = 0;

		bov_text_t *textDraw = bov_text_new((GLubyte[]) {"Hello World ! \xa1"}, GL_STATIC_DRAW);
		bov_text_set_fontsize(textDraw, 0.1);
		bov_text_set_boldness(textDraw, 0.4);
		bov_text_set_outline_width(textDraw, 1.0);
		bov_text_set_color(textDraw, (GLfloat[4]) {0.0, 0.0, 0.0, 1.0});

		while(!bov_window_should_close(window)){
			bov_points_set_width(coordDraw, 0.003);
			bov_points_set_outline_width(coordDraw, 0.002);
			// bov_line_loop_draw(window, coordDraw, 0, nPoints);
			// bov_lines_draw(window, coordDraw, 0, nPoints);

			if (cos(bov_window_get_time(window)*5) < 0) {
				bov_text_set_outline_color(textDraw, (GLfloat[4]) {1.0, 0.2, 0.0, 1.0});
				i = 0;
			} else {
				bov_text_set_outline_color(textDraw, (GLfloat[4]) {0.0, 0.8, 1.0, 1.0});
				if (i==0) {
					bov_text_set_pos(textDraw, (GLfloat[2]) {0.3f * rand() / RAND_MAX - 0.25f,
															0.3f * rand() / RAND_MAX - 0.25f});
					i = 1;
				}
			}
			bov_text_draw(window, textDraw);

			bov_points_set_width(coordDraw, 0.003);
			bov_points_set_outline_width(coordDraw, -1.);
			bov_points_draw(window, coordDraw, 0, nPoints);

			bov_window_update(window);
		}

		int *hull = malloc(sizeof(int)*nPoints);
		jarvis_march(nPoints, coord, hull);

		bov_points_delete(coordDraw);
		bov_text_delete(textDraw);
		free(coord);
		bov_window_delete(window);

		return EXIT_SUCCESS;
	}
*/

int main()
{
	// give a bit of entropy for the seed of rand()
	// or it will always be the same sequence
	int seed = (int) time(NULL);
	srand(seed);

	// we print the seed so you can get the distribution of points back
	printf("seed=%d\n", seed);

	const int nPoints = 10;
	float (*coord)[2] = malloc(sizeof(coord[0])*nPoints);
#if 0 // put 1 for random polygon
	random_polygon(coord, nPoints, 4);
#else
	random_points(coord, nPoints);
#endif

	/*
	int* sorted = (int*)calloc(nPoints, sizeof(int));
	argsort(0, nPoints, coord, 0, sorted);
	for (int i=0; i<nPoints; i++) {
		printf("%d  -- %.2f\n", sorted[i], coord[sorted[i]][0]);
	}
	*/

	printf("Jarvis March Algorithm --- BEGIN\n");
	// printf("Graham's scan Algorithm --- BEGIN\n");
	clock_t t0 = clock();

	int *indexHull = malloc(sizeof(int)*nPoints);

	// int nHull = jarvis_march(nPoints, coord, indexHull);
	// int nHull = graham_scan(nPoints, coord, indexHull);
	int nHull = chan_(nPoints, coord, indexHull, 15);

	float (*coordHull)[2] = malloc(sizeof(coordHull[0])*nHull);
	for (int i=0; i<nHull; i++) {
		coordHull[i][0] = coord[indexHull[i]][0];
		coordHull[i][1] = coord[indexHull[i]][1];
	}

	clock_t t1 = clock();
	double tm_tot = (double) (t1-t0) / CLOCKS_PER_SEC;

	printf("Jarvis March Algorithm --- END\n");
	printf("  -- Number of points on the grid : N = %d\n", nPoints);
	printf("  -- Number of points in the hull : N = %d\n", nHull);
	printf("  -- Time needed for computations : t = %.3e [s]\n", tm_tot);

	const GLsizei nPoints_GL = (GLsizei) nPoints;
	GLfloat (*coord_GL)[2] = (GLfloat (*)[2]) coord;
	GLsizei nHull_GL   = (GLsizei) nHull;
	GLfloat (*coordHull_GL)[2] = (GLfloat (*)[2]) coordHull;


	bov_window_t* window = bov_window_new(800, 800, "Jarvis March Algorithm");
	bov_window_set_color(window, (GLfloat[]){0.9f, 0.85f, 0.8f, 1.0f});

	bov_points_t *coordDraw = bov_points_new(coord_GL, nPoints_GL, GL_STATIC_DRAW);
	bov_points_set_color(coordDraw, (GLfloat[4]) {0.0, 0.0, 0.0, 1.0});
	bov_points_set_outline_color(coordDraw, (GLfloat[4]) {0.3, 0.12, 0.0, 0.25});

	bov_points_t *coordDrawHull = bov_points_new(coordHull_GL, nHull_GL, GL_STATIC_DRAW);
	bov_points_set_color(coordDrawHull, (GLfloat[4]) {0.0, 0.0, 0.0, 1.0});
	bov_points_set_outline_color(coordDrawHull, (GLfloat[4]) {0.3, 0.12, 0.0, 0.25});

	while(!bov_window_should_close(window)){
		bov_points_set_width(coordDraw, 0.003);
		bov_points_set_outline_width(coordDraw, 0.002);
		bov_points_set_width(coordDrawHull, 0.003);
		bov_points_set_outline_width(coordDrawHull, 0.);

		bov_line_loop_draw(window, coordDrawHull, 0, nHull);
		// bov_lines_draw(window, coordDraw, 0, nPoints);

		bov_points_set_width(coordDraw, 0.005);
		bov_points_set_outline_width(coordDraw, -1.);
		bov_points_draw(window, coordDraw, 0, nPoints);

		bov_window_update(window);
	}

	bov_points_delete(coordDraw);
	bov_points_delete(coordDrawHull);
	free(coord); free(coord_GL);
	free(coordHull); free(coordHull_GL);
	bov_window_delete(window);

	return EXIT_SUCCESS;
}
