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

void argsort(int nPoints, float coord[][2], int axis, int* argsorted_list)
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

/*
-------------------------------------------------
Inputs :
	x1,x2,x3  -- Coordinates of 3 points in the plane
		axis 0 : x-direction
		axis 1 : y-direction

Output :
	area      -- Area of the triangle between those 3 points
-------------------------------------------------
*/
static float direction(float x1[2], float x2[2], float x3[2])
{
	float area = (x1[0]*x2[1] - x2[0]*x1[1]) \
			   - (x1[0]*x3[1] - x3[0]*x1[1]) \
			   + (x2[0]*x3[1] - x3[0]*x2[1]);
	return area;
}

static float distance(float x1[2], float x2[2])
{ return sqrt(pow(x1[0]-x2[0],2.) + pow(x1[1]-x2[1],2.)); }


/*
Jarvis March Algorithm
-------------------------------------------------
Inputs :
	nPoints   -- Amount of points in the coord vector
	coord     -- Coordinates of points in the grid
		axis 0 : x-direction
		axis 1 : y-direction
	indexHull -- Array of integer for indexing the points in the convex hull

Output :
	count     -- Amount of points in the convex Hull
-------------------------------------------------
*/
int jarvis_march(int nPoints, float coord[][2], int* indexHull)
{
	/* Initialisation */
	int left,prev,next,count;
	float drct,dst1,dst2;

	/* Step 1 : Take the leftest point */
	left = argmin(nPoints, coord, 0);

	count = 1;
	prev  = left;
	indexHull[0] = left;

	/* Step 2 : while loop
		TAKE the next point in 'coord'
		FOR  each point on the grid :
			VERIFY if we turn left or right
			IF left, change the reference point
			ELSE nothing
		STOP when we come back to the first point
	*/
	while(1 && count<nPoints) {
		next = (prev+1) %nPoints;
		for (int i=0; i<nPoints; i++) {
			drct = direction(coord[prev],coord[i],coord[next]);
			dst1 = distance(coord[prev],coord[i]);
			dst2 = distance(coord[prev],coord[next]);

			if ((drct<0.0) || (drct==0.0 && dst1>dst2)) {
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

/*
Graham's Scan Algorithm
*/

//input: x and y coo of 3 points
//output: >0 if the 3 points turn left (counter-clock) in the 1-2-3 way, <0 otherwise
// non robuste: return 0 if points are aligned
// double turn_dir(float x1, float y1, float x2, float y2, float x3, float y3){
// 	return (x1*(y2-y3) - x2*(y1-y3) + x3*(y1-y2));
// }
double turn_dir(float x1[2], float x2[2], float x3[2]){
	double area = (x1[0]*x2[1] - x2[0]*x1[1]) \
			   - (x1[0]*x3[1] - x3[0]*x1[1]) \
			   + (x2[0]*x3[1] - x3[0]*x2[1]);
	return area;
}


/*
Graham's Scan Algorithm
-------------------------------------------------
Inputs :
	nPoints   -- Amount of points in the coord vector
	coord     -- Coordinates of points in the grid
		axis 0 : x-direction
		axis 1 : y-direction
	indexHull -- Array of integer for indexing the points in the convex hull

Output :
	count     -- Amount of points in the convex Hull
-------------------------------------------------
*/
int graham_scan(int nPoints, float coord[][2], int* hull)
{
	/* Initialisation */
	int ul_tracker,ll_tracker,flag; double drct;
	int* upper_list = (int*)calloc(nPoints, sizeof(int));
	int* lower_list = (int*)calloc(nPoints, sizeof(int));
	flag = 0;

	/* Step 1 : Sort the point wrt x
		Return (int*) sorted[N] */
	int* sorted = (int*)calloc(nPoints, sizeof(int));
	argsort(nPoints, coord, 0, sorted);

	/* Step 2 : Store the 2 leftest points in upper_list */
	upper_list[0] = sorted[0];
	upper_list[1] = sorted[1];
	// tracker of the next free position
	ul_tracker = 2;

	/* Step 3 : FOR 3 → nPoints */
	for( int i = 2; i < nPoints; i++){
		/* Step 3.1 : APPEND point 'i' to upper_list */
		upper_list[ul_tracker] = sorted[i];
		ul_tracker++;


		drct = turn_dir(coord[upper_list[ul_tracker-3]], \
						 coord[upper_list[ul_tracker-2]], \
						 coord[upper_list[ul_tracker-1]]);
		flag = 1;

		/* Step 3.2 : WHILE sizeof(upper_list) > 2
			&& 3 last points make a LEFT turn */
		while( flag &&  drct>=0){
			/* DELETE the middle point */
			upper_list[ul_tracker-2] = upper_list[ul_tracker-1];
			upper_list[ul_tracker-1] = 0;
			ul_tracker--;

			flag = 0;
			if (ul_tracker > 2){
				drct = turn_dir(coord[upper_list[ul_tracker-3]],
								 coord[upper_list[ul_tracker-2]],
								 coord[upper_list[ul_tracker-1]]);
				flag = 1;
			}
		}

	}

	/* Step 4 : Store the points' indexes of the upper convex hull */
	for(int i=0; i<ul_tracker; i++){
		hull[i] = upper_list[i];
	}

	/* Step 5 : Store the 2 rightest points in lower_list */
	lower_list[0] = sorted[nPoints-1];
	lower_list[1] = sorted[nPoints-2];
	// tracker of the next free position
	ll_tracker = 2;

	/* Step 6 : FOR nPoints-3 → 0 */
	for(int i = nPoints-3; i >-1; i--){
		/* Step 6.1 : APPEND point 'i' to lower_list */
		lower_list[ll_tracker] = sorted[i];
		ll_tracker++;

		drct = turn_dir(coord[lower_list[ll_tracker-3]],
						 coord[lower_list[ll_tracker-2]],
						 coord[lower_list[ll_tracker-1]]);
		flag = 1;

		/* Step 6.2 : WHILE sizeof(upper_list) > 2
			&& 3 last points make a LEFT turn */
		while(flag && drct >=0){
			// DELETE the middle point
			lower_list[ll_tracker-2] = lower_list[ll_tracker-1];
			lower_list[ll_tracker-1] = 0;
			ll_tracker--;

			flag = 0;
			if(ll_tracker > 2){
				drct = turn_dir(coord[lower_list[ll_tracker-3]],
								 coord[lower_list[ll_tracker-2]],
								 coord[lower_list[ll_tracker-1]]) / 2.0;
				flag = 1;
			}
		}

	}

	/* Step 7 : Store the points' indexes of the lower convex hull */
	for(int i=1; i<ll_tracker-1; i++){
		hull[ul_tracker-1+i] = lower_list[i];
	}

	/* Termination */
	free(lower_list);
	free(upper_list);

	return ul_tracker + ll_tracker - 2;
}

// DIVIDE AND CONQUER (FOR SPARTA)


int* concat(int* V1, int* V2){
	int length_V1 = sizeof(V1) / sizeof(V1[0]);
	int length_V2 = sizeof(V2) / sizeof(V2[0]);

	int* concat_V = calloc((length_V1+length_V2), sizeof(int));
	for(int i=0; i<length_V1; i++){
		concat_V[i] = V1[i];
	}
	for(int j=0; j<length_V2; j++){
		concat_V[length_V1+j] = V2[j];
	}
	printf("length = %d \n", length_V1 + length_V2);
	return concat_V;
}

int min_dist(int* points, float coord[][2], int I, int J){
	double min_dist=10000; double dist;
	int min_point = -1;

	for(int i=0; i< sizeof(points) / sizeof(points[0]); i++){
		dist = turn_dir(coord[I], coord[J], coord[points[i]]);
		if(dist<min_dist){
			min_dist=dist;
			min_point = points[i];
		}
	}
	return min_point;
}

int* quick_hull(int* S, int size_S, int V_i, int V_j, float coord[][2]){
	int* I = calloc(size_S, sizeof(int));
	int* J = calloc(size_S, sizeof(int));
	int* tab = calloc(2, sizeof(int));
	int size_I = 0; int size_J = 0; int V=0;
	if(size_S==0){
		tab[0] = V_i; tab[1] = V_j;
		return tab;
	} else {
		V = min_dist(S, coord, V_i, V_j);
		for(int i=0; i< size_S; i++){
			if(turn_dir(coord[V_i], coord[V],coord[S[i]]) < 0){
				I[size_I] = S[i];
				size_I++;
			} else if (turn_dir(coord[V], coord[V_j], coord[S[i]]) < 0 ){
				J[size_J] = S[i];
				size_J++;
			} else{

			}
		}

	}
	int* V1 = quick_hull(I, size_I, V_i, V, coord);
	int* V2 = quick_hull(J, size_J, V, V_j, coord);

	int length_V1 = (int)(sizeof(V1) / sizeof(V1[0]));
	int length_V2 = (int)(sizeof(V2) / sizeof(V2[0]));


	int* concat_V = calloc((length_V1+length_V2), sizeof(int));
	for(int i=0; i<length_V1; i++){
		concat_V[i] = V1[i];
	}
	for(int j=0; j<length_V2; j++){
		concat_V[length_V1+j] = V2[j];
	}

	printf("size: %ld, expect: %d \n", sizeof(concat_V)/sizeof(concat_V[0]),length_V1+length_V2);
	return concat_V;
}
