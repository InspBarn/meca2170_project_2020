#include "inputs.h"

/* fonction that outputs a random value, with a
 * probability following a gaussian curve */
float random_gauss(float mu, float sigma)
{
	float u1, u2;
	do {
		u1 = (float) rand() / RAND_MAX;
		u2 = (float) rand() / RAND_MAX;
	} while (u1<1e-6);
	return mu + sigma * sqrtf(-2.0f * logf(u1)) * cosf(2.0f * M_PI * u2);
}


/* fill coord with random coordinates following an uniform distribution */
/*
	void random_uniform_points(GLfloat coord[][2], GLsizei n,
	                           GLfloat min[2], GLfloat max[2])
	{
		for (GLsizei i=0; i<n; i++) {
			coord[i][0] = (max[0] - min[0]) * rand() / RAND_MAX + min[0];
			coord[i][1] = (max[1] - min[1]) * rand() / RAND_MAX + min[1];
		}
	}
*/
void random_uniform_points(float coord[][2], int n,
                           float min[2], float max[2])
{
	for (int i=0; i<n; i++) {
		coord[i][0] = (max[0] - min[0]) * rand() / RAND_MAX + min[0];
		coord[i][1] = (max[1] - min[1]) * rand() / RAND_MAX + min[1];
	}
}


/* creating random points following a gaussian distribution.
 * around multiple centroid (maximum 6 centroids) which
 * are uniformly*/
/*
	void random_points(GLfloat coord[][2], GLsizei n)
	{
		int n_centroids = rand() % 6 + 1;
		GLfloat (*centroids)[2] = malloc(sizeof(GLfloat) * n_centroids * 2);
		GLfloat (*sigma)[2] = malloc(sizeof(GLfloat) * n_centroids * 2);

		GLfloat range = 0.7 * (1.0 - 1.0 / n_centroids);
		random_uniform_points(centroids, n_centroids,
		                      (GLfloat[2]) {-range, -range},
		                      (GLfloat[2]) { range,  range});

		for (GLsizei i=0; i<n_centroids; i++) {
			sigma[i][0] = 0.3f * rand() / RAND_MAX + 0.1f;
			sigma[i][1] = 0.3f * rand() / RAND_MAX + 0.1f;
		}

		for (GLsizei i=0; i<n; i++) {
			for(int j=0; j<2; j++) {
				coord[i][j] = random_gauss(centroids[i%n_centroids][j], sigma[i%n_centroids][j]);
			}
		}

		free(centroids);
		free(sigma);
	}
*/
void random_points(float coord[][2], int n)
{
	int n_centroids = rand() % 6 + 1;
	float (*centroids)[2] = malloc(sizeof(float) * n_centroids * 2);
	float (*sigma)[2] = malloc(sizeof(float) * n_centroids * 2);

	float range = 0.7 * (1.0 - 1.0 / n_centroids);
	random_uniform_points(centroids, n_centroids,
	                      (float[2]) {-range, -range},
	                      (float[2]) { range,  range});

	for (int i=0; i<n_centroids; i++) {
		sigma[i][0] = 0.3f * rand() / RAND_MAX + 0.1f;
		sigma[i][1] = 0.3f * rand() / RAND_MAX + 0.1f;
	}

	for (int i=0; i<n; i++) {
		for(int j=0; j<2; j++) {
			coord[i][j] = random_gauss(centroids[i%n_centroids][j], sigma[i%n_centroids][j]);
		}
	}

	free(centroids);
	free(sigma);
}


// see https://stackoverflow.com/questions/16542042
/*
	static inline GLfloat pseudoangle(GLfloat dx, GLfloat dy)
	{
		GLfloat p = dx/(fabs(dx)+fabs(dy)); // -1 .. 1 increasing with x
		if (dy<0.0f)
			return 3.0f + p;  //  2 .. 4 increasing with x
		else
			return 1.0f - p;      //  0 .. 2 decreasing with x
	}
*/
static inline float pseudoangle(float dx, float dy)
{
	float p = dx/(fabs(dx)+fabs(dy)); // -1 .. 1 increasing with x
	if (dy<0.0f)
		return 3.0f + p;  //  2 .. 4 increasing with x
	else
		return 1.0f - p;  //  0 .. 2 decreasing with x
}


/* compare the angles that two points make
 *
 * The sorting is not robust, there are a possible floating point
 * errors in the pseudoangle function.
 */
/*
	static int compare_angle(const void *a_v, const void *b_v)
	{
		GLfloat* a = *(GLfloat(*)[2]) a_v;
		GLfloat* b = *(GLfloat(*)[2]) b_v;

		GLfloat diff = pseudoangle(b[0], b[1]) - pseudoangle(a[0], a[1]);
		return (diff>0) - (diff<0);
	}
*/
static int compare_angle(const void *a_v, const void *b_v)
{
	float* a = *(GLfloat(*)[2]) a_v;
	float* b = *(GLfloat(*)[2]) b_v;

	float diff = pseudoangle(b[0], b[1]) - pseudoangle(a[0], a[1]);
	return (diff>0) - (diff<0);
}


/* create a random polygon
 * the bigger nSmooth is, the rounder it will be  */
/*
	void random_polygon(GLfloat coord[][2], GLsizei n, int nSmooth)
	{
		GLfloat sigmax = (GLfloat) rand() / RAND_MAX;
		GLfloat sigmay = (GLfloat) rand() / RAND_MAX;

		for(GLsizei i=0; i<n; i++) {
			coord[i][0] = random_gauss(0.0f, sigmax);
			coord[i][1] = random_gauss(0.0f, sigmay);
		}

		qsort(coord, n, 2 * sizeof(GLfloat), compare_angle);

		// a little bit of smoothing
		for(int smoothing=0; smoothing<nSmooth; smoothing++) {
			// because we do not copy the array and do the smoothing in place
			// we start at a random index :p
			int index = rand() % n;
			for(int i=1; i<n-1; i++) {
				coord[(index+i)%n][0] = (2 * coord[(index+i)%n][0] +
				                           coord[(index+i+n-1)%n][0] +
				                           coord[(index+i+1)%n][0]
				                        ) * 0.25f;
				coord[(index+i)%n][1] = ( 2 * coord[(index+i)%n][1] +
				                          coord[(index+i+n-1)%n][1] +
				                          coord[(index+i+1)%n][1]
				                        ) * 0.25f;
			}
		}
	}
*/
void random_polygon(float coord[][2], int n, int nSmooth)
{
	float sigmax = (float) rand() / RAND_MAX;
	float sigmay = (float) rand() / RAND_MAX;

	for(int i=0; i<n; i++) {
		coord[i][0] = random_gauss(0.0f, sigmax);
		coord[i][1] = random_gauss(0.0f, sigmay);
	}

	qsort(coord, n, 2 * sizeof(float), compare_angle);

	// a little bit of smoothing
	for(int smoothing=0; smoothing<nSmooth; smoothing++) {
		// because we do not copy the array and do the smoothing in place
		// we start at a random index :p
		int index = rand() % n;
		for(int i=1; i<n-1; i++) {
			coord[(index+i)%n][0] = (2 * coord[(index+i)%n][0] +
			                           coord[(index+i+n-1)%n][0] +
			                           coord[(index+i+1)%n][0]
			                        ) * 0.25f;
			coord[(index+i)%n][1] = ( 2 * coord[(index+i)%n][1] +
			                          coord[(index+i+n-1)%n][1] +
			                          coord[(index+i+1)%n][1]
			                        ) * 0.25f;
		}
	}
}
