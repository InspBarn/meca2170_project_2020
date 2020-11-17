#include "convex_hull.h"

static inline void insert(int N, int M, int* sorted)
{
	int prev, next;
	next = N;
	for(int a=M; a<N; a++) {
		prev = next;
		next = sorted[a];
		sorted[a] = prev;
	}
	sorted[N] = next;
}

static void argsort(int nPoints, GLfloat* vector, int* argsorted_list)
{
	argsorted_list[0] = 0;
	int i,j; double x;
	for(i=1; i<nPoints; i++) {
		x = vector[i];
		for (j=i; j>0; j--) {
			if (x<vector[argsorted_list[j-1]]) {
				argsorted_list[j] = argsorted_list[j-1];
			} else {break;}
		}
		argsorted_list[j] = i;
	}
}

void jarvis_march(GLsizei nPoints, GLfloat (*coord)[2], int* hull)
{
	GLfloat *x_pos = coord[0];
	GLfloat *y_pos = coord[1];

	int *argsorted_coord;
	argsort(nPoints, x_pos, argsorted_coord);

	int nHull = 0;
	hull[nHull] = argsorted_coord[0];
}

