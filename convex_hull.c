#include "convex_hull.h"
#include <unistd.h>

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

static int argmax(int nPoints, float coord[][2], int axis)
{
	int argmax = 0;
	float max = coord[0][axis];
	for (int i=1; i<nPoints; i++) {
		if (coord[i][axis]>max) {
			argmax = i; max = coord[i][axis];
		}
	}
	return argmax;
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

int empty_hull(int nPoints, float coord[][2], int* indexHull)
{
	indexHull[0] = argmin(nPoints, coord, 0);
	return 1;
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
int jarvis_march(int nPoints, float coord[][2], int* indexHull, bov_window_t* window)
{
	/* Initialisation */
	int left,prev,next,count;
	float drct,dst1,dst2;
	int *indexHull_anim = malloc(sizeof(int)*nPoints);

	/* Step 1 : Take the leftest point */
	left = argmin(nPoints, coord, 0);

	count = 1;
	prev  = left;

	/* Step 2 : while loop
		TAKE the next point in 'coord'
		FOR  each point on the grid :
			VERIFY if we turn left or right
			IF left, change the reference point
			ELSE nothing
		STOP when we come back to the first point
	*/
	while(1 && count<nPoints) {
		indexHull[count-1] = prev;
		indexHull_anim[count-1] = prev;

		next = (prev+1) %nPoints;

		for (int i=0; i<nPoints; i++) {
			drct = direction(coord[prev],coord[i],coord[next]);
			dst1 = distance(coord[prev],coord[i]);
			dst2 = distance(coord[prev],coord[next]);

			if (nPoints < 501) {
				indexHull_anim[count] = i;
				indexHull_anim[count+1] = next;
				animate(window, nPoints, coord, count+2, indexHull_anim);
				if (bov_window_should_close(window)) {
						goto end_of_jarvis;
				}
			}

			if ((drct<0.0) || (drct==0.0 && dst1>dst2)) {
				next =i;
			}
		}
		if (next==left) {break;}

		prev = next;
		count ++;
	}

	indexHull[count] = left;
	while(!bov_window_should_close(window)) {
		animate(window, nPoints, coord, count+1, indexHull);
	}

end_of_jarvis:
	free(indexHull_anim);

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


		drct = direction(coord[upper_list[ul_tracker-3]], \
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
				drct = direction(coord[upper_list[ul_tracker-3]],
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

		drct = direction(coord[lower_list[ll_tracker-3]],
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

// DIVIDE AND CONQUER (FOR SPARTA)


int* concat(int* V1, int* V2)
{
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

int min_dist(int* points, float coord[][2], int I, int J)
{
	double min_dist=10000; double dist;
	int min_point = -1;

	for(int i=0; i< sizeof(points) / sizeof(points[0]); i++){
		dist = direction(coord[I], coord[J], coord[points[i]]);
		if(dist<min_dist){
			min_dist=dist;
			min_point = points[i];
		}
	}
	return min_point;
}

int* quick_hull(int* S, int size_S, int V_i, int V_j, float coord[][2])
{
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
			if(direction(coord[V_i], coord[V],coord[S[i]]) < 0){
				I[size_I] = S[i];
				size_I++;
			} else if (direction(coord[V], coord[V_j], coord[S[i]]) < 0 ){
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

/* ----------------------------------------------
                  CHAN ALGORITHM
---------------------------------------------- */

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
int chan_(int nPoints, float coord[][2], int* indexHull, int mPoints, bov_window_t* window)
{
	/* ------------------------------------------
		GRAHAM'S PARTITION OF THE SET OF POINTS
	--------------------------------------------- */

	/* Initialisation */
	int count = 0;
	int countMax;
	int mSets,mLastPoints;

	/* Step 1 : Evaluate the Amount of Partition Required */
	mSets = nPoints / mPoints;
	mLastPoints = mPoints + nPoints%mPoints;

	/* Step 2 : Creation of all the Partitions into a Vector */
	struct ConvexHull myHulls[mSets];
	for (int i=0; i<mSets-1; i++) {
		_convexHull_init(&myHulls[i], mPoints*i, mPoints*(i+1),
			coord, graham_scan, i%MAXCOLORS);
		countMax = countMax + myHulls[i].nHull;
	}
	_convexHull_init(&myHulls[mSets-1], mPoints*(mSets-1), nPoints,
		coord, graham_scan, (mSets-1)%MAXCOLORS);
	countMax = countMax + myHulls[mSets-1].nHull;

	/* ------------------------------------------
		JARVIS MARCH BETWEEN THE GRAHAM'S PARTITIONS
	--------------------------------------------- */

	/* Initialisation
		left : Leftest Point on the grid
		prev, curr : Two last points of the hull
		next : Point considered to be the next one in the hull
		drct : Direction of (prev,curr,next)
		dst1, dst2 : Distances between (prev,next) & (curr,next) respectively
	*/
	int m,left,prev,curr,next;
	float drct,dst1,dst2;

	// Each step hull -> Ease Lecture
	struct ConvexHull *mySetHull; // = malloc(sizeof(struct ConvexHull));
	int mySetStart,mySetnHull,mySetNext,mySetNext2;
	float mySetDrct,mySetDrct2;

	// Display for Each Subsets
	struct ConvexHull *display = malloc(sizeof(struct ConvexHull));
	_convexHull_init(display, 0, nPoints, coord, empty_hull, -1);
	display->colorDraw = (GLfloat[4]) {0.0,0.0,0.0,1.0};

	/* Step 1 : Take the Leftest Point */
	left = argmin(nPoints, coord, 0);

	prev = myHulls[left/mPoints].Start 
		 + myHulls[left/mPoints].indexHull[myHulls[left/mPoints].nHull];
	curr = left;

	count = 1;
	indexHull[0] = left;
	display->indexHull[0] = left;

	/* Step 2 : Compute the hull point by point
		STOP Condition : The point found to be the next on the hull is left
						 or we tried all the points of the hulls */
	while (count<countMax) {
		/* Step 2.1 : Initialize the next point of the hull */
		m = curr/mPoints;
		mySetHull = &myHulls[m];
		next = mySetHull->Start + mySetHull->indexHull[0];
		for (int i=0; i<mySetHull->nHull; i++) {
			prev = mySetHull->Start + mySetHull->indexHull[i-1];
			if (prev == curr) {
				next = mySetHull->Start + mySetHull->indexHull[i];
				break;
			}
		}

		_convexHull_update(display, next, count+1);
		for (int i=0; i<mSets; i++) {
			// bov_points_draw(window, myHulls[i].coordDraw, 0, myHulls[i].nPoints_GL);
			bov_line_loop_draw(window, myHulls[i].coordDrawHull, 0, myHulls[i].nHull_GL);
		}
		bov_line_strip_draw(window, display->coordDrawHull, 0, display->nHull_GL);
		bov_window_update(window);
		usleep(5000);

		/* Step 2.2 : Find the Point which Minimizes the Angle */
		for (int i=1; i<mSets; i++) {
			mySetHull  = &myHulls[(m+i)%mSets];
			mySetnHull = mySetHull->nHull;
			mySetStart = mySetHull->Start;

			mySetNext  = mySetStart + mySetHull->indexHull[0];
			mySetDrct  = direction(coord[curr],coord[mySetNext],coord[next]);

			for (int j=1; j<mySetnHull; j++) {
				mySetNext2 = mySetStart + mySetHull->indexHull[j];
				mySetDrct2  = direction(coord[curr], coord[mySetNext2], coord[mySetNext]);

				dst1 = distance(coord[curr], coord[mySetNext2]);
				dst2 = distance(coord[curr], coord[mySetNext]);

				if ((mySetDrct2<0.0) || (mySetDrct2==0.0 && dst1>dst2)) {
					mySetNext = mySetNext2;
				}
			}

			drct = direction(coord[curr], coord[mySetNext], coord[next]);
			if (drct<0.0) {
				next = mySetNext;
			}

			_convexHull_update(display, mySetNext, count+1);
			for (int i=0; i<mSets; i++) {
				// bov_points_draw(window, myHulls[i].coordDraw, 0, myHulls[i].nPoints_GL);
				bov_line_loop_draw(window, myHulls[i].coordDrawHull, 0, myHulls[i].nHull_GL);
			}
			bov_line_strip_draw(window, display->coordDrawHull, 0, display->nHull_GL);
			bov_window_update(window);
			usleep(5000);
			if (bov_window_should_close(window)) {
				goto end_of_march;
			}

		}
		if (next==left) {break;}

		indexHull[count] = next;
		// prev = curr;
		curr = next;
		count ++;

		_convexHull_update(display, next, count);
	}

	_convexHull_update(display, -1, count);
	while(!bov_window_should_close(window)) {
		bov_points_draw(window, display->coordDraw, 0, display->nPoints_GL);
		bov_line_loop_draw(window, display->coordDrawHull, 0, display->nHull_GL);
		bov_window_update(window);
	}

end_of_march:
	// free(mySetHull);
	free(display);

	return count;
}

void _convexHull_init(struct ConvexHull *myHull, int start, int stop, float coord[][2], int (*hull_function)(int,  float (*)[2], int *), int color)
{
	myHull->Start = start;
	myHull->Stop  = stop;

	myHull->nPoints = stop-start;

	myHull->coord = (float(*)[2])malloc(sizeof(myHull->coord[0])*myHull->nPoints);
	myHull->indexHull = (int*)malloc(sizeof(int)*myHull->nPoints);
	myHull->coordHull = (float(*)[2])malloc(sizeof(myHull->coord[0])*myHull->nPoints);

	for(int i=0; i<myHull->nPoints; i++) {
		myHull->coord[i][0] = coord[start+i][0];
		myHull->coord[i][1] = coord[start+i][1];
	}

	myHull->nHull = hull_function(myHull->nPoints, myHull->coord, myHull->indexHull);
	for (int i=0; i<myHull->nHull; i++) {
		myHull->coordHull[i][0] = myHull->coord[myHull->indexHull[i]][0];
		myHull->coordHull[i][1] = myHull->coord[myHull->indexHull[i]][1];
	}

	myHull->nPoints_GL = (GLsizei) myHull->nPoints;
	myHull->coord_GL = (GLfloat (*)[2]) myHull->coord;
	myHull->coordDraw = bov_points_new(myHull->coord_GL, myHull->nPoints_GL, GL_STATIC_DRAW);

	myHull->nHull_GL = (GLsizei) myHull->nHull;
	myHull->coordHull_GL = (GLfloat (*)[2]) myHull->coordHull;
	myHull->coordDrawHull = bov_points_new(myHull->coordHull_GL, myHull->nHull_GL, GL_STATIC_DRAW);

	GLfloat colorDraw[4];
	colorDraw[3] = (GLfloat) 1.0;
	if (color==-1) {
		// BLACK (0, 0, 0)
		myHull->colorDraw = (GLfloat[4]) {0.0, 0.0, 0.0, 1.0};
	} else if (color==0) {
		// MEDIUMVIOLETRED (199, 21, 133)
		myHull->colorDraw = (GLfloat[4]) {199.0/255.0, 21.0/255.0, 133.0/255.0, 1.0};
	} else if (color==1) {
		// FORESTGREEN (34, 139, 34)
		myHull->colorDraw = (GLfloat[4]) {34.0/255.0, 139.0/255.0, 34.0/255.0, 1.0};
	} else if (color==2) {
		// FIREBRICK (178, 34, 34)
		myHull->colorDraw = (GLfloat[4]) {178.0/255.0, 34.0/255.0, 34.0/255.0, 1.0};
	} else if (color==3) {
		// BLUE (0, 0, 255)
		myHull->colorDraw = (GLfloat[4]) {0.0/255.0, 0.0/255.0, 255.0/255.0, 1.0};
	} else if (color==4) {
		// DARKORANGE (255, 99, 71)
		myHull->colorDraw = (GLfloat[4]) {255.0/255.0, 99.0/255.0, 71.0/255.0, 1.0};
	} else if (color==5) {
		// DEEPPINK (255, 20, 147)
		myHull->colorDraw = (GLfloat[4]) {255.0/255.0, 20.0/255.0, 147.0/255.0, 1.0};
	} else if (color==6) {
		// CHARTREUSE (127, 255, 0)
		myHull->colorDraw = (GLfloat[4]) {127.0/255.0, 255.0/255.0, 0.0/255.0, 1.0};
	} else if (color==7) {
		// CRIMSON (220, 20, 60)
		myHull->colorDraw = (GLfloat[4]) {220.0/255.0, 20.0/255.0, 60.0/255.0, 1.0};
	} else if (color==8) {
		// DEEPSKYBLUE (0, 191, 255)
		myHull->colorDraw = (GLfloat[4]) {0.0/255.0, 191.0/255.0, 255.0/255.0, 1.0};
	} else if (color==9) {
		// GOLD (255, 215, 0)
		myHull->colorDraw = (GLfloat[4]) {255.0/255.0, 215.0/255.0, 0.0/255.0, 1.0};
	} else if (color==10) {
		// HOTPINK (255, 105, 180)
		myHull->colorDraw = (GLfloat[4]) {255.0/255.0, 105.0/255.0, 180.0/255.0, 1.0};
	} else if (color==11) {
		// MEDIUMSPRINGGREEN (0, 250, 154)
		myHull->colorDraw = (GLfloat[4]) {0.0/255.0, 250.0/255.0, 154.0/255.0, 1.0};
	} else if (color==12) {
		// INDIANRED (205, 92, 92)
		myHull->colorDraw = (GLfloat[4]) {205.0/255.0, 92.0/255.0, 92.0/255.0, 1.0};
	} else if (color==13) {
		// STEELBLUE (70, 130, 180)
		myHull->colorDraw = (GLfloat[4]) {70.0/255.0, 130.0/255.0, 180.0/255.0, 1.0};
	} else if (color==14) {
		// PINK (255, 192, 203)
		myHull->colorDraw = (GLfloat[4]) {255.0/255.0, 192.0/255.0, 203.0/255.0, 1.0};
	}

	bov_points_set_color(myHull->coordDraw, myHull->colorDraw);
	bov_points_set_outline_color(myHull->coordDraw, (GLfloat[4]) {0.3, 0.12, 0.0, 0.25});
	bov_points_set_width(myHull->coordDraw, POINTS_WIDTH);
	bov_points_set_outline_width(myHull->coordDraw, POINTS_OUTLINE_WIDTH);
	// bov_points_set_outline_width(myHull->coordDraw, -1.0);

	bov_points_set_color(myHull->coordDrawHull, myHull->colorDraw);
	bov_points_set_outline_color(myHull->coordDrawHull, (GLfloat[4]) {0.3, 0.12, 0.0, 0.25});
	bov_points_set_width(myHull->coordDrawHull, POINTS_WIDTH);
	bov_points_set_outline_width(myHull->coordDrawHull, POINTS_OUTLINE_WIDTH);
	// bov_points_set_outline_width(myHull->coordDrawHull, -1.0);
}

void _convexHull_update(struct ConvexHull *myHull, int nextPoint, int nextCount)
{
	if (nextPoint!=-1) {
		myHull->indexHull[nextCount-1] = nextPoint;

		myHull->coordHull[nextCount-1][0] = myHull->coord[nextPoint][0];
		myHull->coordHull[nextCount-1][1] = myHull->coord[nextPoint][1];
	}
	myHull->nHull = nextCount;

	myHull->nHull_GL = (GLsizei) myHull->nHull;
	myHull->coordHull_GL = (GLfloat (*)[2]) myHull->coordHull;
	myHull->coordDrawHull = bov_points_new(myHull->coordHull_GL, myHull->nHull_GL, GL_STATIC_DRAW);

	bov_points_set_color(myHull->coordDrawHull, myHull->colorDraw);
	bov_points_set_outline_color(myHull->coordDrawHull, (GLfloat[4]) {0.3, 0.12, 0.0, 0.25});
	bov_points_set_width(myHull->coordDrawHull, POINTS_WIDTH);
	bov_points_set_outline_width(myHull->coordDrawHull, POINTS_OUTLINE_WIDTH);
	// bov_points_set_outline_width(myHull->coordDrawHull, -1.0);
}

void _convexHull_free(struct ConvexHull *myHull)
{
	free(myHull->colorDraw);

	free(myHull->coord);
	free(myHull->coord_GL);
	bov_points_delete(myHull->coordDraw);

	free(myHull->indexHull);
	free(myHull->coordHull);
	free(myHull->coordHull_GL);
	bov_points_delete(myHull->coordDrawHull);

	free(myHull);
}

// PLOT
void animate(bov_window_t* window, int nPoints, float coord[][2], int nHull, int* indexHull)
{
	GLfloat *CHARTREUSE = (GLfloat[4]) {127.0/255.0, 1.0, 0.0, 1.0};
	GLfloat *BLACK = (GLfloat[4]) {0.0, 0.0, 0.0, 1.0};

	float (*coordHull)[2] = malloc(sizeof(coordHull[0])*nHull);
	for (int i=0; i<nHull; i++) {
		coordHull[i][0] = coord[indexHull[i]][0];
		coordHull[i][1] = coord[indexHull[i]][1];
	}

	const GLsizei nPoints_GL = (GLsizei) nPoints;
	GLfloat (*coord_GL)[2] = (GLfloat (*)[2]) coord;
	GLsizei nHull_GL = (GLsizei) nHull;
	GLfloat (*coordHull_GL)[2] = (GLfloat (*)[2]) coordHull;

	bov_points_t *coordDraw = bov_points_new(coord_GL, nPoints_GL, GL_STATIC_DRAW);
	bov_points_set_color(coordDraw, BLACK);
	bov_points_set_outline_color(coordDraw, (GLfloat[4]) {0.3, 0.12, 0.0, 0.25});
	bov_points_set_width(coordDraw, 0.005);
	bov_points_set_outline_width(coordDraw, -1.0);

	bov_points_t *coordDrawHull = bov_points_new(coordHull_GL, nHull_GL, GL_STATIC_DRAW);
	bov_points_set_color(coordDrawHull, CHARTREUSE);
	bov_points_set_outline_color(coordDrawHull, (GLfloat[4]) {0.3, 0.12, 0.0, 0.25});
	bov_points_set_width(coordDrawHull, 0.005);
	bov_points_set_outline_width(coordDrawHull, -1.0);

	//bov_line_loop_draw(window, coordDrawHull, 0, nHull);
	bov_line_strip_draw(window, coordDrawHull, 0, nHull);
	bov_points_draw(window, coordDraw, 0, nPoints);
	bov_window_update(window);

	bov_points_delete(coordDraw);
	bov_points_delete(coordDrawHull);
}
