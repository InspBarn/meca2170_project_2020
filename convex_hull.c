#include "convex_hull.h"

/*
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
*/

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
	int ul_tracker,ll_tracker,flag; float drct;
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

		/* Pass through only if there is a 
			minimum of 3 points in the upper hull */
		if (ul_tracker > 2){
			drct = direction(coord[upper_list[ul_tracker-3]], \
							 coord[upper_list[ul_tracker-2]], \
							 coord[upper_list[ul_tracker-1]]) / 2.0;
			flag = 1;
		} // Condition indispensable ??

		/* Step 3.2 : WHILE sizeof(upper_list) > 2 
			&& 3 last points make a LEFT turn */
		while( flag &&  drct>0.0){
			/* DELETE the middle point */
			upper_list[ul_tracker-2] = upper_list[ul_tracker-1];
			upper_list[ul_tracker-1] = 0;
			ul_tracker--;

			flag = 0;
			if (ul_tracker > 2){
				drct = direction(coord[upper_list[ul_tracker-3]],
								 coord[upper_list[ul_tracker-2]],
								 coord[upper_list[ul_tracker-1]]) / 2.0;
				flag = 1;
			}
			if(i==nPoints-1){
				printf("HERE: %d et  %f \n", flag, drct);
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
	for(int i = nPoints-3; i > -1; i--){
		/* Step 6.1 : APPEND point 'i' to lower_list */
		lower_list[ll_tracker] = sorted[i];
		ll_tracker++;

		flag = 0;
		/* Pass through only if there is a 
			minimum of 3 points in the lower hull */
		if(ll_tracker > 2){
			drct = direction(coord[lower_list[ll_tracker-3]],
							 coord[lower_list[ll_tracker-2]],
							 coord[lower_list[ll_tracker-1]]) / 2.0;
			flag = 1;
		} // Condition indispensable ??

		/* Step 6.2 : WHILE sizeof(upper_list) > 2 
			&& 3 last points make a LEFT turn */
		while(flag && drct >0.0){
			// DELETE the middle point
			lower_list[ll_tracker-2] = lower_list[ll_tracker-1];
			lower_list[ll_tracker-1] = 0;
			ll_tracker--;

			flag = 0;
			if(ll_tracker > 2){
				drct = direction(coord[lower_list[ll_tracker-3]],
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

/*
Chan's Algorithm
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
int chan_(int nPoints, float coord[][2], int* hull, int mPoints)
{
	int mSets,mLastPoints;
	// mPoints = 10;
	// mSets = nPoints /mPoints;
	// mLastPoints = mPoints + nPoints%mPoints;

	mSets = nPoints / mPoints;
	mLastPoints = mPoints + nPoints%mPoints;

	ConvexHull *myHull1 = malloc(sizeof(ConvexHull));
	ConvexHull *myHull2 = malloc(sizeof(ConvexHull));
	_convexHull_init(myHull1, 0, mPoints, coord);
	_convexHull_init(myHull2, mPoints, nPoints, coord);

	printf("Hello World!\n");
	myHull1->indexHull = malloc(myHull1->nPoints);
	myHull2->indexHull = malloc(myHull2->nPoints);
	myHull1->nHull = graham_scan(myHull1->nPoints, myHull1->coord, myHull1->indexHull);
	myHull2->nHull = graham_scan(myHull2->nPoints, myHull2->coord, myHull2->indexHull);

	int left = argmin(nPoints, coord, 0);

	// int mHull1 = graham_scan(mPoints, coord[])

	// int (*mHull)[] = malloc(sizeof([0])*mPoints)
	return 0;
}


void _convexHull_init(ConvexHull *myHull, int start, int stop, float coord[][2])
{
	myHull->Start = start;
	myHull->Stop  = stop;

	myHull->nPoints = stop-start;
	myHull->coord = malloc(sizeof(coord[0])*myHull->nPoints);
	for(int i=0; i<myHull->nPoints; i++) {
		myHull->coord[i][0] = coord[start+i][0];
		myHull->coord[i][1] = coord[start+i][1];
	}

	myHull->left = argmin(myHull->nPoints, myHull->coord, 0);
	myHull->argsorted = malloc(myHull->nPoints);
	argsort(myHull->nPoints, myHull->coord, 0, myHull->argsorted);
}
