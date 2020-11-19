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

// Graham's scan part

//input: x and y coo of 3 points
//output: >0 if the 3 points turn left (counter-clock) in the 1-2-3 way, <0 otherwise
// non robuste: return 0 if points are aligned
double turn_dir(float x1, float y1, float x2, float y2, float x3, float y3){
	return (1.0/2.0)*(x1*(y2-y3) - x2*(y1-y3) + x3*(y1-y2));
}

int graham_scan(float (*coord)[2], int* hull, int N){

	int nHull = 0;

	// 1) sort the point wrt x, return (int*) sorted[N]
	int* sorted = (int*)calloc(N, sizeof(int));
	argsort(N,coord, 0, sorted);

	//2) p1 and p2 in upper_list
	int* upper_list = (int*)calloc(N, sizeof(int));

	upper_list[0] = sorted[0];
	upper_list[1] = sorted[1];
	int ul_tracker = 2; // tracker of the next free position
	double turn;
	int flag = 0;

	// for 3 to n
	for( int i = 2; i < N; i++){

		// append pi to upper_list
		upper_list[ul_tracker] = sorted[i];
		ul_tracker++;

		if (ul_tracker > 2){
			turn = turn_dir(coord[upper_list[ul_tracker-3]][0], coord[upper_list[ul_tracker-3]][1],
											coord[upper_list[ul_tracker-2]][0], coord[upper_list[ul_tracker-2]][1],
											coord[upper_list[ul_tracker-1]][0], coord[upper_list[ul_tracker-1]][1]);

			flag = 1;
		}

		// while sizeof(upper_list) > 2 && 3 last points make a LEFT turn
		while( flag &&  turn>0.0){

			//delete middle point
			upper_list[ul_tracker-2] = upper_list[ul_tracker-1];
			upper_list[ul_tracker-1] = 0;
			ul_tracker--;


			flag = 0;
			if (ul_tracker > 2){
				turn = turn_dir(coord[upper_list[ul_tracker-3]][0], coord[upper_list[ul_tracker-3]][1],
												coord[upper_list[ul_tracker-2]][0], coord[upper_list[ul_tracker-2]][1],
												coord[upper_list[ul_tracker-1]][0], coord[upper_list[ul_tracker-1]][1]);
				flag = 1;
			}
			if(i==N-1){
				printf("HERE: %d et  %f \n", flag, turn);
			}
		}

	}

	int* lower_list = (int*)calloc(N, sizeof(int));

	lower_list[0] = sorted[N-1];
	lower_list[1] = sorted[N-2];
	int ll_tracker = 2; // tracker of the next free position

	for( int i = N-3; i >-1; i--){

		// append pi to lower_list
		lower_list[ll_tracker] = sorted[i];
		ll_tracker++;

		flag = 0;
		if(ll_tracker > 2){
			turn = turn_dir(coord[lower_list[ll_tracker-3]][0], coord[lower_list[ll_tracker-3]][1],
											coord[lower_list[ll_tracker-2]][0], coord[lower_list[ll_tracker-2]][1],
											coord[lower_list[ll_tracker-1]][0], coord[lower_list[ll_tracker-1]][1]);
			flag = 1;
		}

		// while sizeof(upper_list) > 2 && 3 last points make a LEFT turn
		while(flag && turn >0.0){
			//delete middle point
			lower_list[ll_tracker-2] = lower_list[ll_tracker-1];
			lower_list[ll_tracker-1] = 0;
			ll_tracker--;

			flag = 0;
			if(ll_tracker > 2){
				turn = turn_dir(coord[lower_list[ll_tracker-3]][0], coord[lower_list[ll_tracker-3]][1],
												coord[lower_list[ll_tracker-2]][0], coord[lower_list[ll_tracker-2]][1],
												coord[lower_list[ll_tracker-1]][0], coord[lower_list[ll_tracker-1]][1]);
				flag = 1;
			}
		}

	}

	for(int j=0; j<ul_tracker; j++){
		hull[j] = upper_list[j];
		nHull++;
	}

	for(int k=1; k<ll_tracker-1; k++){
		hull[ul_tracker-1+k] = lower_list[k];
		nHull++;
	}

	free(lower_list);
	free(upper_list);

	return nHull;

}
