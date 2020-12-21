#include "convex_hull.h"

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
float direction(float x1[2], float x2[2], float x3[2])
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
#define JARVIS_ANIMATION 0

void jarvis_march_anim(bov_window_t *window, struct convex_hull_t *display, int end_of)
{
	bov_points_set_color(display->coordDraw, (GLfloat[4]) CHARTREUSE);
	if (end_of)
		bov_line_loop_draw_with_order(window, display->coordDraw, display->hullDraw, 0, display->nHull_GL);
	else
		bov_line_strip_draw_with_order(window, display->coordDraw, display->hullDraw, 0, display->nHull_GL);
	bov_points_set_color(display->coordDraw, (GLfloat[4]) BLACK);
	bov_points_draw(window, display->coordDraw, 0, display->nPoints_GL);
	bov_window_update(window);
}

struct convex_hull_t* jarvis_march(int nPoints, float coord[][2])
{
	/* Initialisation */
	int left,prev,curr,next,count;
	float drct,dst1,dst2;
	clock_t t0,t1;

#if JARVIS_ANIMATION
	bov_window_t* window = bov_window_new(800, 800, "Jarvis March Algorithm");
	bov_window_set_color(window, (GLfloat[]){0.9f, 0.85f, 0.8f, 1.0f});
#endif

	struct convex_hull_t *result = malloc(sizeof(struct convex_hull_t));
	convex_hull_init(result, 0, nPoints, coord, JARVIS_ANIMATION);
	int *hull_idxs = calloc(nPoints, sizeof(int));

	/* Step 1 : Take the leftest point */
	left = argmin(nPoints, coord, 0);
	next = left;
	count = 0;

	/* Step 2 : While the convex hull is not complete .. */
	t0 = clock();
	while(1 && count<nPoints) {
		/* Step 2.1 : Storage */
		hull_idxs[count] = next;
		prev = next; count ++;

#if JARVIS_ANIMATION
		if (nPoints>100) {
			convex_hull_update(result, hull_idxs, count);
			jarvis_march_anim(window, result, 0);
			usleep(TIME_STEP);
			if (bov_window_should_close(window))
				goto end_of_jarvis;
		}
#endif

		/* Step 2.2 : Break if we come back to left
					  Take next point in the list */
		if ((next==left) && (count>1))
			break;

		next = (prev+1) %nPoints;
		hull_idxs[count+1] = next;
		// convex_hull_update(result, hull_idxs, count+2);

		/* Step 2.3 : For each point of the list .. */
		for (int i=1; i<nPoints; i++) {
			curr = (prev+i+1)%nPoints;
			/* Step 2.3.1 : Compute the direction and the distances */
			drct = direction(coord[prev],coord[curr],coord[next]);
			dst1 = distance(coord[prev],coord[curr]);
			dst2 = distance(coord[prev],coord[next]);

#if JARVIS_ANIMATION
			if (nPoints<=100) {
				hull_idxs[count] = curr;
				// convex_hull_partial_update(result, nextD, count, count+2, 0);
				convex_hull_update(result, hull_idxs, count+2);
				jarvis_march_anim(window, result, 0);
				if (bov_window_should_close(window))
					goto end_of_jarvis;
			}
#endif

			/* Step 2.3.2 : The point is next in the convex hull */
			if ((drct<0.0) || (drct==0.0 && dst1>dst2)) {
				next = curr; hull_idxs[count+1] = curr;
			}
		}
	}
	t1 = clock();

	result->method = "Jarvis March";
	result->time = (double) (t1-t0) / CLOCKS_PER_SEC;
	convex_hull_update(result, hull_idxs, count+1);

end_of_jarvis:
	free(hull_idxs);
#if JARVIS_ANIMATION
	bov_window_delete(window);
#endif
	return result;
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
#define GRAHAM_ANIMATION 0

struct convex_hull_t* graham_scan(int nPoints, float coord[][2])
{
	// bov_window_t* window = bov_window_new(800, 800, "Graham Scan Algorithm");

	/* Initialisation */
	clock_t t0,t1;
	int tracker,flag; float drct;
	int *hull_idxs = (int*)calloc(nPoints, sizeof(int));
	flag = 0;

#if GRAHAM_ANIMATION
	bov_window_t* window = bov_window_new(800, 800, "Graham Scan Algorithm");
	bov_window_set_color(window, (GLfloat[]){0.9f, 0.85f, 0.8f, 1.0f});
#endif

	struct convex_hull_t *result = malloc(sizeof(struct convex_hull_t));
	convex_hull_init(result, 0, nPoints, coord, GRAHAM_ANIMATION);

	/* Step 1 : Sort the point wrt x
		Return (int*) sorted[N] */
	int* sorted = (int*)calloc(nPoints, sizeof(int));
	argsort(nPoints, coord, 0, sorted);

	/* Step 2 : Store the 2 leftest points in upper_list */
	hull_idxs[0] = sorted[0]; hull_idxs[1] = sorted[1];
	// tracker of the next free position
	tracker = 2;

	t0 = clock();
	/* Step 3 : FOR 3 → nPoints */
	for( int i = 2; i < nPoints; i++){
		/* Step 3.1 : APPEND point 'i' to upper_list */
		hull_idxs[tracker] = sorted[i];
		tracker ++;

#if GRAHAM_ANIMATION
		convex_hull_update(result, hull_idxs, tracker);
		jarvis_march_anim(window, result, 0);
		usleep(TIME_STEP);
		// if (bov_window_should_close(window))
		// 	goto end_of_jarvis;
#endif

		flag = 1;
		drct = direction(coord[hull_idxs[tracker-3]], \
						 coord[hull_idxs[tracker-2]], \
						 coord[hull_idxs[tracker-1]]);

		/* Step 3.2 : WHILE sizeof(upper_list) > 2
			&& 3 last points make a LEFT turn */
		while( flag &&  drct>=0){
			/* DELETE the middle point */
			hull_idxs[tracker-2] = hull_idxs[tracker-1];
			hull_idxs[tracker-1] = 0;
			tracker--;

#if GRAHAM_ANIMATION
			convex_hull_update(result, hull_idxs, tracker);
			jarvis_march_anim(window, result, 0);
			usleep(TIME_STEP);
			// if (bov_window_should_close(window))
			// 	goto end_of_jarvis;
#endif

			flag = 0;
			if (tracker > 2){
				flag = 1;
				drct = direction(coord[hull_idxs[tracker-3]],
								 coord[hull_idxs[tracker-2]],
								 coord[hull_idxs[tracker-1]]);
			}
			//Animate
			// animate(coord, window, upper_list, ul_tracker, nPoints);
		}
	}

	int up_tracker = tracker-1;

	/* Step 4 : Store the 2 rightest points in lower_list */
	// hull_idxs[tracker] = sorted[nPoints-1];
	hull_idxs[tracker] = sorted[nPoints-2];
	// tracker of the next free position
	tracker = tracker+1;

	/* Step 5 : FOR nPoints-3 → 0 */
	for(int i=nPoints-3; i >-1; i--){
		/* Step 5.1 : APPEND point 'i' to lower_list */
		hull_idxs[tracker] = sorted[i];
		tracker++;

#if GRAHAM_ANIMATION
		convex_hull_update(result, hull_idxs, tracker);
		jarvis_march_anim(window, result, 0);
		usleep(TIME_STEP);
		// if (bov_window_should_close(window))
		// 	goto end_of_jarvis;
#endif

		flag = 1;
		drct = direction(coord[hull_idxs[tracker-3]],
						 coord[hull_idxs[tracker-2]],
						 coord[hull_idxs[tracker-1]]);

		/* Step 5.2 : WHILE sizeof(upper_list) > 2
			&& 3 last points make a LEFT turn */
		while(flag && drct >=0){
			// DELETE the middle point
			hull_idxs[tracker-2] = hull_idxs[tracker-1];
			hull_idxs[tracker-1] = 0;
			tracker--;

#if GRAHAM_ANIMATION
			convex_hull_update(result, hull_idxs, tracker);
			jarvis_march_anim(window, result, 0);
			usleep(TIME_STEP);
			// if (bov_window_should_close(window))
			// 	goto end_of_jarvis;
#endif

			flag = 0;
			if(tracker-up_tracker > 2){
				flag = 1;
				drct = direction(coord[hull_idxs[tracker-3]],
								 coord[hull_idxs[tracker-2]],
								 coord[hull_idxs[tracker-1]]);
			}
		}
	}
	t1 = clock();

	result->method = "Graham Scan";
	result->time = (double) (t1-t0) / CLOCKS_PER_SEC;
	convex_hull_update(result, hull_idxs, tracker-1);

	/* Termination */
	free(sorted);
	free(hull_idxs);
#if GRAHAM_ANIMATION
	bov_window_delete(window);
#endif
	return result;
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

int max_dist(int* points, int size_points, float coord[][2], int I, int J){
	double max_dist=-10000; float dist;
	int max_point = -1;

	for(int i=0; i< size_points; i++){
		dist = direction(coord[I], coord[J], coord[points[i]]);
		if(dist>max_dist){
			max_dist=dist;
			max_point = points[i];
		}
	}
	return max_point;
}

int min_dist(int* points, int size_points, float coord[][2], int I, int J){
	double min_dist=10000; float dist;
	int min_point = -1;

	for(int i=0; i< size_points; i++){
		dist = direction(coord[I], coord[J], coord[points[i]]);
		if(dist<min_dist){
			min_dist=dist;
			min_point = points[i];
		}
	}
	return min_point;
}

int quick_hull(int* S, int size_S, int V_i, int V_j, float coord[][2], int* return_hull, int flag_left){
	int* I = calloc(size_S, sizeof(int));
	int* J = calloc(size_S, sizeof(int));
	int* tab = calloc(2, sizeof(int));
	int size_I = 0; int size_J = 0; int V=0;
	if(size_S==0){
		return_hull[0] = V_i; return_hull[1] = V_j;
		return 2;
	}else if(flag_left==1){
		V = max_dist(S, size_S, coord, V_i, V_j);
		for(int i=0; i< size_S; i++){
			if(direction(coord[V_i], coord[V],coord[S[i]]) > 0){
				I[size_I] = S[i];
				size_I++;
			} else if (direction(coord[V], coord[V_j], coord[S[i]]) > 0 ){
				J[size_J] = S[i];
				size_J++;
			}
		}

		int* V1 = calloc(size_I+2, sizeof(int));
		int* V2 = calloc(size_J+2, sizeof(int));

		int length_V1 = quick_hull(I, size_I, V_i, V, coord, V1, 1);
		int length_V2 = quick_hull(J, size_J, V, V_j, coord, V2, 1);

		for(int i=0; i<length_V1; i++){
			return_hull[i] = V1[i];
		}
		for(int j=0; j<length_V2; j++){
			return_hull[length_V1+j] = V2[j];
		}
		return length_V1+length_V2;
	}else{
		V = min_dist(S, size_S, coord, V_i, V_j);
		for(int i=0; i< size_S; i++){
			if(direction(coord[V_i], coord[V],coord[S[i]]) < 0){
				I[size_I] = S[i];
				size_I++;
			} else if (direction(coord[V], coord[V_j], coord[S[i]]) < 0 ){
				J[size_J] = S[i];
				size_J++;
			}
		}
		int* V1 = calloc(size_I+2, sizeof(int));
		int* V2 = calloc(size_J+2, sizeof(int));

		int length_V1 = quick_hull(I, size_I, V_i, V, coord, V1, 0);
		int length_V2 = quick_hull(J, size_J, V, V_j, coord, V2, 0);

		for(int i=0; i<length_V1; i++){
			return_hull[i] = V1[i];
		}
		for(int j=0; j<length_V2; j++){
			return_hull[length_V1+j] = V2[j];
		}
		return length_V1+length_V2;
	}
}

// PLOT
void animate(float coord[][2], bov_window_t* window, int* actual_hull, int nHull, int nPoints){
	//int nHull = sizeof(actual_hull)/sizeof(actual_hull[0]);

	float (*coordHull)[2] = malloc(sizeof(coordHull[0])*nHull);
	for (int i=0; i<nHull; i++) {
		coordHull[i][0] = coord[actual_hull[i]][0];
		coordHull[i][1] = coord[actual_hull[i]][1];
	}

	GLsizei nHull_GL   = (GLsizei) nHull;
	GLfloat (*coordHull_GL)[2] = (GLfloat (*)[2]) coordHull;

	const GLsizei nPoints_GL = (GLsizei) nPoints;
	GLfloat (*coord_GL)[2] = (GLfloat (*)[2]) coord;

	const bov_order_t *coordDrawHull = bov_order_new((GLuint*) actual_hull, nHull_GL, GL_DYNAMIC_DRAW);
	// bov_points_set_color(coordDrawHull, (GLfloat[4]) {1.0, 0.6, 0.3, 1.0});
	// bov_points_set_outline_color(coordDrawHull, (GLfloat[4]) {0.3, 0.12, 0.0, 0.25});

	while(!bov_window_should_close(window)){
		bov_points_t *coordDraw = bov_points_new(coord_GL, nPoints_GL, GL_STATIC_DRAW);
		bov_points_set_width(coordDraw, 0.003);
		bov_points_set_outline_width(coordDraw, 0.);


		//bov_line_loop_draw(window, coordDrawHull, 0, nHull);
		bov_line_strip_draw_with_order(window, coordDraw, coordDrawHull, 0, nHull);

		bov_points_set_color(coordDraw, (GLfloat[4]) {0.0, 0.0, 0.0, 1.0});
		bov_points_set_outline_color(coordDraw, (GLfloat[4]) {0.3, 0.12, 0.0, 0.25});

		bov_points_draw(window, coordDraw, 0, nPoints);


		bov_window_update(window);
		sleep(0.1);
		break;
	}

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
#define CHAN_PRESENTATION_ 0
#define CHAN_ANIMATION 0
#if CHAN_ANIMATION
#define CHAN_PRESENTATION CHAN_PRESENTATION_
#else
#define CHAN_PRESENTATION 0
#endif

struct convex_hull_t* chan_(int nPoints, float coord[][2])
{
	int mPoints = 40;
	/* ------------------------------------------
		GRAHAM'S PARTITION OF THE SET OF POINTS
	--------------------------------------------- */

	/* Initialisation */
	int count = 0;
	int countMax;
	int mSets,mLastPoints;
	clock_t t0,t1;

#if CHAN_ANIMATION
	bov_window_t* window = bov_window_new(800, 800, "Chan Algorithm");
	bov_window_set_color(window, (GLfloat[]){0.9f, 0.85f, 0.8f, 1.0f});
#endif

	// Display for Each Subsets
	struct convex_hull_t *result = malloc(sizeof(struct convex_hull_t));
	convex_hull_init(result, 0, nPoints, coord, CHAN_ANIMATION);

	/* Step 1 : Evaluate the Amount of Partition Required */
	mSets = nPoints / mPoints;
	mLastPoints = nPoints%mPoints;
	if (mLastPoints==0)
		mLastPoints = mPoints;
	else
		mSets ++;

	/* Step 2 : Creation of all the Partitions into a Vector */
	struct convex_hull_t myHulls[mSets];
	float (*coord_)[2] = malloc(sizeof(coord_[0])*mPoints);
	for (int i=0; i<mSets-1; i++) {
		for (int j=0; j<mPoints; j++) {
			coord_[j][0] = coord[mPoints*i+j][0];
			coord_[j][1] = coord[mPoints*i+j][1];
		}
		myHulls[i] = *graham_scan(mPoints, coord_);
		myHulls[i].Start = i*mPoints;
		countMax = countMax + myHulls[i].nHull;
	}

	for (int j=0; j<mLastPoints; j++) {
		coord_[j][0] = coord[mPoints*(mSets-1)+j][0];
		coord_[j][1] = coord[mPoints*(mSets-1)+j][1];
	}
	myHulls[mSets-1] = *graham_scan(mLastPoints, coord_);
	myHulls[mSets-1].Start = (mSets-1)*mPoints;
	countMax = countMax + myHulls[mSets-1].nHull;

	int *hull_idxs = calloc(countMax, sizeof(int));

#if CHAN_ANIMATION
	for (int i==0; i<mSets; i++)
		convex_hull_display_init(&myHulls[i], i%MAXCOLORS);
#endif

#if CHAN_PRESENTATION
	bov_points_draw(window, result->coordDraw, 0, result->nPoints_GL);
	bov_window_update(window);
	for (int i=0; i<mSets; i++) {
		bov_points_draw(window, result->coordDraw, 0, result->nPoints_GL);
		bov_points_draw(window, myHulls[i].coordDraw, 0, myHulls[i].nPoints_GL);
		bov_line_loop_draw_with_order(window, myHulls[i].coordDraw, myHulls[i].hullDraw, 0, myHulls[i].nHull_GL);
		bov_window_update(window);

		clock_t t0 = clock(), t1 = clock();
		while ((double) (t1-t0) / CLOCKS_PER_SEC < 2.0)
			t1 = clock();
	}
#endif

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

	/* Step 1 : Take the Leftest Point */
	left = argmin(nPoints, coord, 0);
	next = left;
	count = 0;
	hull_idxs[0] = left;
	// result->hull_idxs[0] = left;

	/* Step 2 : Compute the hull point by point
		STOP Condition : The point found to be the next on the hull is left
						 or we tried all the points of the hulls */
	t0 = clock();
	while (count<countMax) {
		/* Step 2.1 : Storage */
		hull_idxs[count] = next;
		curr = next;
		count ++;

		/* Step 2.2 : Break if we come back to left
					  Take next point of the current hull */
		if ((next==left) && (count>1))
			break;

		m = curr/mPoints;
		mySetHull = &myHulls[m];
		next = mySetHull->Start + mySetHull->hull_idxs[0];
		for (int i=1; i<mySetHull->nHull; i++) {
			prev = mySetHull->Start + mySetHull->hull_idxs[i-1];
			if (prev == curr) {
				next = mySetHull->Start + mySetHull->hull_idxs[i];
				break;
			}
		}

#if CHAN_ANIMATION
		hull_idxs[count] = next;
		convex_hull_update(result, hull_idxs, count+1);

		for (int i=0; i<mSets; i++)
			bov_line_loop_draw_with_order(window, myHulls[i].coordDraw, myHulls[i].hullDraw, 0, myHulls[i].nHull_GL);
		bov_line_strip_draw_with_order(window, result->coordDraw, result->hullDraw, 0, result->nHull_GL);
		bov_window_update(window);
		usleep(TIME_STEP);

		if (bov_window_should_close(window))
			goto end_of_march;
#endif

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

#if CHAN_ANIMATION
			// convex_hull_partial_update(result, &mySetNext, count, count+1, count+1);
			hull_idxs[count] = mySetNext;
			convex_hull_update(result, hull_idxs, count+1);

			for (int i=0; i<mSets; i++)
				bov_line_loop_draw_with_order(window, myHulls[i].coordDraw, myHulls[i].hullDraw, 0, myHulls[i].nHull_GL);
			bov_line_strip_draw_with_order(window, result->coordDraw, result->hullDraw, 0, result->nHull_GL);
			bov_window_update(window);
			usleep(TIME_STEP);
#endif

			drct = direction(coord[curr], coord[mySetNext], coord[next]);
			if (drct<0.0)
				next = mySetNext;
		}
	}
	t1 = clock();
	convex_hull_update(result, hull_idxs, count);

	result->method = "Chan";
	result->time = (double) (t1-t0) / CLOCKS_PER_SEC;

	convex_hull_update(result, hull_idxs, count);

end_of_march:
	free(hull_idxs);
#if CHAN_ANIMATION
	bov_window_delete(window);
#endif
	return result;
}

void convex_hull_init(struct convex_hull_t *myHull, int start, int stop, float coord[][2], int display)
{
	myHull->display = display;
	myHull->Start = start;
	myHull->nPoints = stop-start;

	myHull->coord = (float(*)[2])malloc(sizeof(myHull->coord[0])*myHull->nPoints);
	myHull->hull_idxs = (int*)malloc(sizeof(int)*myHull->nPoints);

	for(int i=0; i<myHull->nPoints; i++) {
		myHull->coord[i][0] = coord[start+i][0];
		myHull->coord[i][1] = coord[start+i][1];
	}
	myHull->nHull = 1;
	myHull->hull_idxs[0] = 0;
	// myHull->nHull = hull_function(myHull->nPoints, myHull->coord, myHull->hull_idxs);

	if (display)
		convex_hull_display_init(myHull, -1);
}

void convex_hull_display_init(struct convex_hull_t *hull, int color)
{
	hull->nPoints_GL = (GLsizei) hull->nPoints;
	hull->nHull_GL = (GLsizei) hull->nHull;
	if (color==-1) {
		hull->coordDraw = bov_points_new((GLfloat (*)[2]) hull->coord, hull->nPoints_GL, GL_DYNAMIC_DRAW);
		hull->hullDraw = bov_order_new((GLuint*) hull->hull_idxs, hull->nHull_GL, GL_DYNAMIC_DRAW);
	} else {
		hull->coordDraw = bov_points_new((GLfloat (*)[2]) hull->coord, hull->nPoints_GL, GL_STATIC_DRAW);
		hull->hullDraw = bov_order_new((GLuint*) hull->hull_idxs, hull->nHull_GL, GL_STATIC_DRAW);
	}

	GLfloat colorDraw[4];
	colorDraw[3] = (GLfloat) 1.0;
	if (color==-1) { hull->colorDraw = (GLfloat[4]) BLACK; }
	else if (color==0) { hull->colorDraw = (GLfloat[4]) MEDIUMVIOLETRED; }
	else if (color==1) { hull->colorDraw = (GLfloat[4]) FORESTGREEN; }
	else if (color==2) { hull->colorDraw = (GLfloat[4]) FIREBRICK; }
	else if (color==3) { hull->colorDraw = (GLfloat[4]) BLUE; }
	else if (color==4) { hull->colorDraw = (GLfloat[4]) DARKORANGE; }
	else if (color==5) { hull->colorDraw = (GLfloat[4]) DEEPPINK; }
	else if (color==6) { hull->colorDraw = (GLfloat[4]) CHARTREUSE; }
	else if (color==7) { hull->colorDraw = (GLfloat[4]) CRIMSON; }
	else if (color==8) { hull->colorDraw = (GLfloat[4]) DEEPSKYBLUE; }
	else if (color==9) { hull->colorDraw = (GLfloat[4]) GOLD; }
	else if (color==10) { hull->colorDraw = (GLfloat[4]) HOTPINK; }
	else if (color==11) { hull->colorDraw = (GLfloat[4]) MEDIUMSPRINGGREEN; }
	else if (color==12) { hull->colorDraw = (GLfloat[4]) INDIANRED; }
	else if (color==13) { hull->colorDraw = (GLfloat[4]) STEELBLUE; }
	else if (color==14) { hull->colorDraw = (GLfloat[4]) PINK; }

	bov_points_set_color(hull->coordDraw, hull->colorDraw);
	bov_points_set_outline_color(hull->coordDraw, (GLfloat[4]) POINTS_OUTLINE_COLOR);
	bov_points_set_width(hull->coordDraw, POINTS_WIDTH);
	bov_points_set_outline_width(hull->coordDraw, POINTS_OUTLINE_WIDTH);
	// bov_points_set_outline_width(hull->coordDraw, -1.0);
}

void convex_hull_update(struct convex_hull_t *hull, const int *idxs, int n)
{
	hull->nHull = n;
	for (int i=0; i<n; i++)
		hull->hull_idxs[i] = idxs[i];

	if (hull->display) {
		hull->nHull_GL = (GLsizei) n;
		hull->hullDraw = bov_order_update(hull->hullDraw,
											(GLuint*) idxs,
											(GLsizei) n);
	}
}

void convex_hull_partial_update(struct convex_hull_t *hull, const int *idxs, int start, int count, int newN)
{
	hull->nHull = newN;
	for (int i=start; i<start+count; i++)
		hull->hull_idxs[i] = idxs[i-start];

	if (hull->display) {
		hull->nHull_GL = (GLsizei) newN;
		hull->hullDraw = bov_order_partial_update(hull->hullDraw,
													(GLuint*) idxs,
													(GLint) start,
													(GLsizei) count,
													(GLsizei) newN);
	}
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

		convex_hull_init(hull_new, 0, nPoints, coord, 1);

		/*
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
		*/
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

		convex_hull_init(hull_new, 0, nPoints, coord, 1);

		int idh = argfind(hull->nHull, hull->hull_idxs, idx);
		if (idh!=-1) {
			/* The considered 'point' is in the hull
				→ Delete it from hull_idxs
				→ Does the hull need to be changed ?
			*/
		}

		convex_hull_update(hull_new, hull->hull_idxs, hull->nHull);
	}

	hull_new->time = hull->time;
	return hull_new;
}

void convex_hull_display(bov_window_t *window, struct convex_hull_t *hull)
{
	int flag = 1;
	float newPoint[2];
	double time, last_time, mouse_right;

	if (!hull->display) {
		hull->display = 1;
		convex_hull_display_init(hull, -1);
	}

	last_time = bov_window_get_time(window);
	mouse_right = window->clickTime[1];

	while (!bov_window_should_close(window)) {
		jarvis_march_anim(window, hull, 1);

		time = bov_window_get_time(window);
		if ((time-last_time>1.0) && !flag){
			flag = 1; mouse_right = window->clickTime[1];
		} else if ((window->clickTime[1] != mouse_right) && flag) {
			newPoint[0] = (float)window->cursorPos[0] - (float)window->param.translate[0];
			newPoint[1] = (float)window->cursorPos[1] - (float)window->param.translate[1];
			hull = convex_hull_click_update(hull, newPoint);

			flag = 0; last_time = bov_window_get_time(window);
		}
	}
}
