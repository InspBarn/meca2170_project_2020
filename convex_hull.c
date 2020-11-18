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

static int argmin(int nPoints, float coord[][2], int axis)
{
	int argmin = 0;
	float min = coord[0][axis];
	for (int i=1; i<nPoints; i++) {
		if (coord[i][axis]<min) {
			argmin = i; min = coord[i][axis];
		}
	}
	return argmin;
}

static void argsort(int nPoints, float coord[][2], int axis, int* argsorted_list)
{
	argsorted_list[0] = 0;
	int i,j; float x;
	for(i=1; i<nPoints; i++) {
		x = coord[i][axis];
		for (j=i; j>0; j--) {
			if (x<coord[argsorted_list[j-1]][axis]) {
				argsorted_list[j] = argsorted_list[j-1];
			} else {break;}
		}
		argsorted_list[j] = i;
	}
}

static int direction(float x1[2], float x2[2], float x3[2])
{
	float area = (x1[0]*x2[1] - x2[0]*x1[1]) \
			   - (x1[0]*x3[1] - x3[0]*x1[1]) \
			   + (x2[0]*x3[1] - x3[0]*x2[1]);

	if (area > 0) {
		return 1;
	} else if (area < 0) {
		return -1;
	} else {
		return 0;
	}
}

static float distance(float x1[2], float x2[2])
{ return sqrt(pow(x1[0]-x2[0],2.) + pow(x1[1]-x2[1],2.)); }


int jarvis_march(int nPoints, float coord[][2], int* indexHull)
{
	int left,prev,next,count;
	float drct,dst1,dst2;

	left = argmin(nPoints, coord, 0);

	count = 1;
	prev  = left;
	indexHull[0] = left;

	int i;
	while(1 && count<nPoints) {
		next = (prev+1) %nPoints;
		for (i=0; i<nPoints; i++) {
			drct = direction(coord[prev],coord[i],coord[next]);
			dst1 = distance(coord[prev],coord[i]);
			dst2 = distance(coord[prev],coord[next]);

			if ((drct==-1) || (drct==0 && dst1>dst2)) {
				next =i;
			}
		}
		if (next==left) {break;}

		indexHull[count] = next;
		prev = next;
		count ++;
	}
	return count;
}

