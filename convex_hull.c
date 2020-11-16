#include "convex_hull.h"

void jarvis_march(GLsizei N, GLfloat (*coord)[2], int* hull)
{
	GLfloat *x_pos = coord[0];
	GLfloat *y_pos = coord[1];
	// argsort(N, x_pos, hull);
	for(GLint i=0; i<nPoints; i++) {
		//printf("%.2f\t%d\n",coord[0][i],hull[i]);
		printf("%.2f\t%.2f\n",coord[0][i],coord[1][i]);
	}
}

static void argsort(GLsizei N, GLfloat* vector, int* sorted)
{
	GLfloat x;
	for(int i=0; i<N; i++) {
		x = vector[i];

		int j = 0;
		while(x>vector[sorted[j]]) {j++;}
		
		if(j==i) {sorted[i] = i;}
		else {insert(i,j, sorted);}
	}
}

static inline void insert(int N, int M, int* sorted)
{
	int prev, next;
	prev = N;
	for(int a=M; a<N-1; a++) {
		next = sorted[a];
		sorted[a] = prev;
		prev = next;
	}
	sorted[N] = prev;
}

