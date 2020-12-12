#include "convex_hull.h"
#include <unistd.h>

static float distance(const float x1[2], const float x2[2]) {
	return sqrt(pow(x1[0]-x2[0],2.) + pow(x1[1]-x2[1],2.));
}

static int find(int nPoints, float coord[][2], const float point[2])
{
	float eps = 5e-3, dst;
	for(int i=0; i<nPoints; i++)
		if (distance(coord[i], point) < eps)
			return i;
	return -1;
}

static void delete(int nPoints, float coord[][2], int idx)
{
	for(int i=idx+1; i<nPoints; i++) {
		coord[i-1][0] = coord[i][0];
		coord[i-1][1] = coord[i][1];
	}
}

static void insert(int nPoints, int *vec, int arg, int idx)
{
	int prev, next;
	prev = arg;
	for(int i=idx; i<nPoints; i++) {
		next = vec[i];
		vec[i] = prev;
		prev = next;
	}
	vec[nPoints] = next;
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

static int argfind(int nPoints, int *vec, int idx)
{
	for(int i=0; i<nPoints; i++)
		if(idx==vec[i])
			return i;
	return -1;
}

static void argshift(int nPoints, int *vec, int arg)
{
	for(int i=0; i<nPoints; i++)
		if(vec[i]>arg)
			vec[i] = vec[i]-1;
}

static void argdelete(int nPoints, int *vec, int arg)
{
	int passed = 0;
	for(int i=0; i<nPoints; i++) {
		if (passed)
			vec[i-1] = vec[i];
		if (!passed && (vec[i]==arg))
			passed = 1;
	}
}

int empty_hull(int nPoints, float coord[][2], int* hull_idxs)
{
	hull_idxs[0] = argmin(nPoints, coord, 0);
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
void jarvis_march_anim(bov_window_t *window, struct convex_hull_t *display, int end_of)
{
	bov_points_set_color(display->coordDraw, (GLfloat[4]) BLACK);
	bov_points_draw(window, display->coordDraw, 0, display->nPoints_GL);
	bov_points_set_color(display->coordDraw, (GLfloat[4]) CHARTREUSE);
	if (end_of)
		bov_line_loop_draw_with_order(window, display->coordDraw, display->hullDraw, 0, display->nHull_GL);
	else
		bov_line_strip_draw_with_order(window, display->coordDraw, display->hullDraw, 0, display->nHull_GL);
	bov_window_update(window);
}

int jarvis_march(int nPoints, float coord[][2], int* hull_idxs)
{
	/* Initialisation */
	int left,prev,curr,next,count;
	float drct,dst1,dst2;

	int showfig = 1;
	int *nextD  = calloc(2, sizeof(int));
	bov_window_t* window = bov_window_new(800, 800, "Jarvis March Algorithm");
	bov_window_set_color(window, (GLfloat[]){0.9f, 0.85f, 0.8f, 1.0f});

	struct convex_hull_t *display = malloc(sizeof(struct convex_hull_t));
	convex_hull_init(display, 0, nPoints, coord, empty_hull, -1);

	/* Step 1 : Take the leftest point */
	left = argmin(nPoints, coord, 0);
	next = left;
	count = 0;

	/* Step 2 : While the convex hull is not complete .. */
	while(1 && count<nPoints) {
		/* Step 2.1 : Storage */
		hull_idxs[count] = next;
		convex_hull_update(display, hull_idxs, count+1);
		prev = next; count ++;

		/* -- Animation -- */
		if (showfig && (nPoints>100)) {
			jarvis_march_anim(window, display, 0);
			usleep(TIME_STEP);
			if (bov_window_should_close(window))
				goto end_of_jarvis;
		}

		/* Step 2.2 : Break if we come back to left
					  Take next point in the list */
		if ((next==left) && (count>1))
			break;
		next = (prev+1) %nPoints;

		hull_idxs[count]   = next;
		hull_idxs[count+1] = next;
		convex_hull_update(display, hull_idxs, count+2);

		/* Step 2.3 : For each point of the list .. */
		for (int i=1; i<nPoints; i++) {
			curr = (prev+i+1)%nPoints;

			/* Step 2.3.1 : Compute the direction and the distances */
			drct = direction(coord[prev],coord[curr],coord[next]);
			dst1 = distance(coord[prev],coord[curr]);
			dst2 = distance(coord[prev],coord[next]);

			/* -- Animation -- */
			if (showfig && (nPoints<=100)) {
				hull_idxs[count] = curr;
				// convex_hull_partial_update(display, nextD, count, count+2, 0);
				convex_hull_update(display, hull_idxs, count+2);
				jarvis_march_anim(window, display, 0);
				if (bov_window_should_close(window))
					goto end_of_jarvis;
			}

			/* Step 2.3.2 : The point is next in the convex hull */
			if ((drct<0.0) || (drct==0.0 && dst1>dst2)) {
				next = curr;
				hull_idxs[count+1] = curr;
			}
		}
	}

	int flag = 1;
	float newPoint[2];
	double time, last_time, mouse_right;

	last_time = bov_window_get_time(window);
	mouse_right = window->clickTime[1];

	while(!bov_window_should_close(window)) {
		jarvis_march_anim(window, display, 1);

		time = bov_window_get_time(window);
		if ((time-last_time>1.0) && !flag){
			flag = 1; mouse_right = window->clickTime[1];
		} else if ((window->clickTime[1] != mouse_right) && flag) {
			newPoint[0] = (float)window->cursorPos[0] - (float)window->param.translate[0];
			newPoint[1] = (float)window->cursorPos[1] - (float)window->param.translate[1];
			display = convex_hull_click_update(display, newPoint);

			flag = 0; last_time = bov_window_get_time(window);
		}
	}

end_of_jarvis:
	free(display);
	bov_window_delete(window);

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
	for(int i = 2; i < nPoints; i++){
		/* Step 3.1 : APPEND point 'i' to upper_list */
		upper_list[ul_tracker] = sorted[i];
		ul_tracker++;


		drct = direction(coord[upper_list[ul_tracker-3]], \
						 coord[upper_list[ul_tracker-2]], \
						 coord[upper_list[ul_tracker-1]]);
		flag = 1;

		/* Step 3.2 : WHILE sizeof(upper_list) > 2
			&& 3 last points make a LEFT turn */
		while(flag &&  drct>=0){
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
int chan_(int nPoints, float coord[][2], int* hull_idxs, int mPoints) //, bov_window_t* window)
{
	/* ------------------------------------------
		GRAHAM'S PARTITION OF THE SET OF POINTS
	--------------------------------------------- */

	/* Initialisation */
	int count = 0;
	int countMax;
	int mSets,mLastPoints;

	int showfig = 0;
	bov_window_t* window = bov_window_new(800, 800, "Chan Algorithm");
	bov_window_set_color(window, (GLfloat[]){0.9f, 0.85f, 0.8f, 1.0f});

	/* Step 1 : Evaluate the Amount of Partition Required */
	mSets = nPoints / mPoints;
	mLastPoints = nPoints%mPoints;
	if (mLastPoints==0)
		mLastPoints = mPoints;
	else
		mSets ++;

	/* Step 2 : Creation of all the Partitions into a Vector */
	struct convex_hull_t myHulls[mSets];
	for (int i=0; i<mSets-1; i++) {
		convex_hull_init(&myHulls[i], mPoints*i, mPoints*(i+1),
			coord, graham_scan, i%MAXCOLORS);
		countMax = countMax + myHulls[i].nHull;
	}
	convex_hull_init(&myHulls[mSets-1], mPoints*(mSets-1), nPoints,
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
	struct convex_hull_t *mySetHull; // = malloc(sizeof(struct ConvexHull));
	int mySetStart,mySetnHull,mySetNext,mySetNext2;
	float mySetDrct,mySetDrct2;

	// Display for Each Subsets
	struct convex_hull_t *display = malloc(sizeof(struct convex_hull_t));
	convex_hull_init(display, 0, nPoints, coord, empty_hull, -1);

	/* Step 1 : Take the Leftest Point */
	left = argmin(nPoints, coord, 0);
	next = left;
	count = 0;
	hull_idxs[0] = left;
	display->hull_idxs[0] = left;

	/* Step 2 : Compute the hull point by point
		STOP Condition : The point found to be the next on the hull is left
						 or we tried all the points of the hulls */
	while (count<countMax) {

		hull_idxs[count] = next;
		curr = next;
		count ++;

		if ((next==left) && (count>1))
			break;

		/* Step 2.1 : Initialize the next point of the hull */
		m = curr/mPoints;
		mySetHull = &myHulls[m];
		next = mySetHull->Start + mySetHull->hull_idxs[0];
		for (int i=0; i<mySetHull->nHull; i++) {
			prev = mySetHull->Start + mySetHull->hull_idxs[i-1];
			if (prev == curr) {
				next = mySetHull->Start + mySetHull->hull_idxs[i];
				break;
			}
		}

		if (showfig) {
			hull_idxs[count] = next;
			convex_hull_update(display, hull_idxs, count+1);

			for (int i=0; i<mSets; i++)
				bov_line_loop_draw_with_order(window, myHulls[i].coordDraw, myHulls[i].hullDraw, 0, myHulls[i].nHull_GL);
			bov_line_strip_draw_with_order(window, display->coordDraw, display->hullDraw, 0, display->nHull_GL);
			bov_window_update(window);
			usleep(TIME_STEP);
		}

		/* Step 2.2 : Find the Point which Minimizes the Angle */
		for (int i=1; i<mSets; i++) {
			mySetHull  = &myHulls[(m+i)%mSets];
			mySetnHull = mySetHull->nHull;
			mySetStart = mySetHull->Start;

			mySetNext  = mySetStart + mySetHull->hull_idxs[0];
			mySetDrct  = direction(coord[curr],coord[mySetNext],coord[next]);

			for (int j=1; j<mySetnHull; j++) {
				mySetNext2 = mySetStart + mySetHull->hull_idxs[j];
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

			if (showfig) {
				// convex_hull_partial_update(display, &mySetNext, count, count+1, count+1);
				hull_idxs[count] = mySetNext;
				convex_hull_update(display, hull_idxs, count+1);

				for (int i=0; i<mSets; i++)
					bov_line_loop_draw_with_order(window, myHulls[i].coordDraw, myHulls[i].hullDraw, 0, myHulls[i].nHull_GL);
				bov_line_strip_draw_with_order(window, display->coordDraw, display->hullDraw, 0, display->nHull_GL);
				bov_window_update(window);
				usleep(TIME_STEP);
			}
		}

		if (bov_window_should_close(window)) {
			goto end_of_march;
		}
	}
	
	convex_hull_update(display, hull_idxs, count);

	while(!bov_window_should_close(window)) {
		bov_points_draw(window, display->coordDraw, 0, display->nPoints_GL);
		bov_line_loop_draw_with_order(window, display->coordDraw, display->hullDraw, 0, display->nHull_GL);
		bov_window_update(window);
	}

end_of_march:
	// free(mySetHull);
	free(display);

	return count;
}

void convex_hull_init(struct convex_hull_t *myHull, int start, int stop, float coord[][2], int (*hull_function)(int,  float (*)[2], int *), int color)
{
	myHull->Start = start;
	myHull->Stop  = stop;

	myHull->nPoints = stop-start;

	myHull->coord = (float(*)[2])malloc(sizeof(myHull->coord[0])*myHull->nPoints);
	myHull->hull_idxs = (int*)malloc(sizeof(int)*myHull->nPoints);

	for(int i=0; i<myHull->nPoints; i++) {
		myHull->coord[i][0] = coord[start+i][0];
		myHull->coord[i][1] = coord[start+i][1];
	}

	myHull->nHull = hull_function(myHull->nPoints, myHull->coord, myHull->hull_idxs);
	myHull->nPoints_GL = (GLsizei) myHull->nPoints;
	myHull->nHull_GL = (GLsizei) myHull->nHull;

	if (color==-1) {
		myHull->coordDraw = bov_points_new((GLfloat (*)[2]) myHull->coord, myHull->nPoints_GL, GL_DYNAMIC_DRAW);
		myHull->hullDraw = bov_order_new((GLuint*) myHull->hull_idxs, myHull->nHull_GL, GL_DYNAMIC_DRAW);
	} else {
		myHull->coordDraw = bov_points_new((GLfloat (*)[2]) myHull->coord, myHull->nPoints_GL, GL_STATIC_DRAW);
		myHull->hullDraw = bov_order_new((GLuint*) myHull->hull_idxs, myHull->nHull_GL, GL_STATIC_DRAW);
	}

	GLfloat colorDraw[4];
	colorDraw[3] = (GLfloat) 1.0;
	if (color==-1) { myHull->colorDraw = (GLfloat[4]) BLACK; }
	else if (color==0) { myHull->colorDraw = (GLfloat[4]) MEDIUMVIOLETRED; }
	else if (color==1) { myHull->colorDraw = (GLfloat[4]) FORESTGREEN; }
	else if (color==2) { myHull->colorDraw = (GLfloat[4]) FIREBRICK; }
	else if (color==3) { myHull->colorDraw = (GLfloat[4]) BLUE; }
	else if (color==4) { myHull->colorDraw = (GLfloat[4]) DARKORANGE; }
	else if (color==5) { myHull->colorDraw = (GLfloat[4]) DEEPPINK; }
	else if (color==6) { myHull->colorDraw = (GLfloat[4]) CHARTREUSE; }
	else if (color==7) { myHull->colorDraw = (GLfloat[4]) CRIMSON; }
	else if (color==8) { myHull->colorDraw = (GLfloat[4]) DEEPSKYBLUE; }
	else if (color==9) { myHull->colorDraw = (GLfloat[4]) GOLD; }
	else if (color==10) { myHull->colorDraw = (GLfloat[4]) HOTPINK; }
	else if (color==11) { myHull->colorDraw = (GLfloat[4]) MEDIUMSPRINGGREEN; }
	else if (color==12) { myHull->colorDraw = (GLfloat[4]) INDIANRED; }
	else if (color==13) { myHull->colorDraw = (GLfloat[4]) STEELBLUE; }
	else if (color==14) { myHull->colorDraw = (GLfloat[4]) PINK; }

	bov_points_set_color(myHull->coordDraw, myHull->colorDraw);
	bov_points_set_outline_color(myHull->coordDraw, (GLfloat[4]) POINTS_OUTLINE_COLOR);
	bov_points_set_width(myHull->coordDraw, POINTS_WIDTH);
	bov_points_set_outline_width(myHull->coordDraw, POINTS_OUTLINE_WIDTH);
	// bov_points_set_outline_width(myHull->coordDraw, -1.0);
}

void convex_hull_update(struct convex_hull_t *hull, const int *idxs, int n)
{
	hull->nHull = n;
	for (int i=0; i<n; i++)
		hull->hull_idxs[i] = idxs[i];

	hull->nHull_GL = (GLsizei) n;
	hull->hullDraw = bov_order_update(hull->hullDraw, 
										(GLuint*) idxs,
										(GLsizei) n);
}

void convex_hull_partial_update(struct convex_hull_t *hull, const int *idxs, int start, int count, int newN)
{
	hull->nHull = newN;
	for (int i=start; i<start+count; i++)
		hull->hull_idxs[i] = idxs[i-start];

	hull->nHull_GL = (GLsizei) newN;
	hull->hullDraw = bov_order_partial_update(hull->hullDraw,
												(GLuint*) idxs,
												(GLint) start,
												(GLsizei) count,
												(GLsizei) newN);
}

struct convex_hull_t* convex_hull_click_update(struct convex_hull_t *hull, const float point[2])
{
	struct convex_hull_t *hull_new = malloc(sizeof(struct convex_hull_t));

	int idx = find(hull->nPoints, hull->coord, point);
	if (idx==-1) {
		/* The considered 'point' is not in my set 'coord' yet
			→ Add it to the end of 'coord'
			→ Does it need to be added to the hull ?
		*/

		int nPoints = hull->nPoints +1;
		float (*coord)[2] = calloc(nPoints, sizeof(coord[0]));
		for (int i=0; i<nPoints-1; i++) {
			coord[i][0] = hull->coord[i][0];
			coord[i][1] = hull->coord[i][1];
		}
		coord[nPoints-1][0] = point[0];		
		coord[nPoints-1][1] = point[1];

		convex_hull_init(hull_new, 0, nPoints, coord, empty_hull, -1);

		nPoints = hull->nHull+1;
		int *hull_idxs = calloc(nPoints, sizeof(int));
		coord[0][0] = point[0]; coord[0][1] = point[1];
		for (int i=0; i<hull->nHull; i++) {
			coord[i+1][0] = hull->coord[hull->hull_idxs[i]][0];
			coord[i+1][1] = hull->coord[hull->hull_idxs[i]][1];
		}

		hull_new->nHull = graham_scan(nPoints, coord, hull_idxs);
		for (int i=0; i<hull_new->nHull; i++) {
			if (hull_idxs[i]==0)
				hull_new->hull_idxs[i] = hull_new->nPoints-1;
			else
				hull_new->hull_idxs[i] = hull->hull_idxs[hull_idxs[i]-1];
		}
		hull_new->nHull_GL = (GLsizei) hull_new->nHull;
		hull_new->hullDraw = bov_order_update(hull_new->hullDraw, 
											(GLuint*) hull_new->hull_idxs,
											(GLsizei) hull_new->nHull_GL);
	} else {
		/* The considered 'point' is in my set 'coord'
			→ Was it part of the convex hull ?
			→ Delete it from 'coord'
		*/

		int nPoints = hull->nPoints -1;
		float (*coord)[2] = calloc(nPoints, sizeof(coord[0]));
		for (int i=0; i<nPoints+1; i++) {
			if (i<idx) {
				coord[i][0] = hull->coord[i][0];
				coord[i][1] = hull->coord[i][1];
			} else if (i>idx) {
				coord[i-1][0] = hull->coord[i][0];
				coord[i-1][1] = hull->coord[i][1];
			}
		}

		int nHull = hull->nHull;
		int *hull_idxs = hull->hull_idxs;
		for (int i=0; i<nHull; i++) {
			if (hull->hull_idxs[i] > idx)
				hull_idxs[i] = hull_idxs[i] -1;
		}

		convex_hull_init(hull_new, 0, nPoints, coord, empty_hull, -1);

		int idh = argfind(hull->nHull, hull->hull_idxs, idx);
		if (idh!=-1) {
			/* The considered 'point' is in the hull
				→ Delete it from hull_idxs
				→ Does the hull need to be changed ?
			*/
		}

		convex_hull_update(hull_new, hull->hull_idxs, hull->nHull);
	}

	return hull_new;
}
