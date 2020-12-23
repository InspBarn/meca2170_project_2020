#include "convex_hull.h"

void jarvis_march_anim(bov_window_t *window, struct convex_hull_t *hull, int end_of)
{
	bov_points_set_color(hull->coordDraw, (GLfloat[4]) BLACK);
	bov_points_draw(window, hull->coordDraw, 0, hull->nPoints_GL);
	bov_points_set_color(hull->coordDraw, (GLfloat[4]) CHARTREUSE);
	if (end_of)
		bov_line_loop_draw_with_order(window, hull->coordDraw, hull->hullDraw, 0, hull->nHull_GL);
	else
		bov_line_strip_draw_with_order(window, hull->coordDraw, hull->hullDraw, 0, hull->nHull_GL);
	bov_window_update(window);
}

struct convex_hull_t* jarvis_march(int nPoints, float coord[][2], int display)
{
#if JARVIS_ANIMATION
	bov_window_t* window;
	if (display) {
		window = bov_window_new(800, 800, "Jarvis March Algorithm");
		bov_window_set_color(window, (GLfloat[]){0.9f, 0.85f, 0.8f, 1.0f});
	}
#endif

	/* Initialisation */
	struct convex_hull_t *result = malloc(sizeof(struct convex_hull_t));
	convex_hull_init(result, 0, nPoints, coord, JARVIS_ANIMATION);
	int *hull_idxs = calloc(nPoints, sizeof(int));

	clock_t t0 = clock();
	int left,prev,curr,next,count;
	float dst1,dst2; double drct;

	/* Step 1 : Take the leftest point */
	left = argmin(nPoints, coord, 0);
	next = left;
	count = 0;

	/* Step 2 : While the convex hull is not complete .. */
	while(1 && count<nPoints) {
		/* Step 2.1 : Storage */
		hull_idxs[count] = next;
		prev = next; count ++;

#if JARVIS_ANIMATION
		if (display && nPoints>100) {
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
			if (display && nPoints<=100) {
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
	clock_t t1 = clock();

	result->method = "Jarvis March";
	result->time = (double) (t1-t0) / CLOCKS_PER_SEC;
	convex_hull_update(result, hull_idxs, count+1);

end_of_jarvis:
	free(hull_idxs);
#if JARVIS_ANIMATION
	if (display) {
		sleep(2);
		result->display = 0;
		bov_points_delete(result->coordDraw);
		bov_order_delete(result->hullDraw);
		bov_window_delete(window);
	}
#endif
	return result;
}


struct convex_hull_t* graham_scan(int nPoints, float coord[][2], int display)
{
#if GRAHAM_ANIMATION
	bov_window_t* window;
	if (display) {
		window = bov_window_new(800, 800, "Graham Scan Algorithm");
		bov_window_set_color(window, (GLfloat[]){0.9f, 0.85f, 0.8f, 1.0f});
	}
#endif
	/* Initialisation */
	struct convex_hull_t *result = malloc(sizeof(struct convex_hull_t));
	convex_hull_init(result, 0, nPoints, coord, GRAHAM_ANIMATION);

	clock_t t0 = clock();
	int tracker,flag; double drct;
	int *hull_idxs = (int*)calloc(nPoints, sizeof(int));
	flag = 0;

	/* Step 1 : Sort the point wrt x
		Return (int*) sorted[N] */
	int* sorted = (int*)calloc(nPoints, sizeof(int));
	argsort(nPoints, coord, 0, sorted);

	/* Step 2 : Store the 2 leftest points in upper_list */
	hull_idxs[0] = sorted[0]; hull_idxs[1] = sorted[1];
	// tracker of the next free position
	tracker = 2;

	/* Step 3 : FOR 3 → nPoints */
	for( int i = 2; i < nPoints; i++){
		/* Step 3.1 : APPEND point 'i' to upper_list */
		hull_idxs[tracker] = sorted[i];
		tracker ++;

#if GRAHAM_ANIMATION
		if (display) {
			convex_hull_update(result, hull_idxs, tracker);
			jarvis_march_anim(window, result, 0);
			usleep(TIME_STEP);
			// if (bov_window_should_close(window))
			// 	goto end_of_graham;
		}
#endif

		flag = 1;
		drct = direction(coord[hull_idxs[tracker-3]], \
						 coord[hull_idxs[tracker-2]], \
						 coord[hull_idxs[tracker-1]]);

		/* Step 3.2 : WHILE sizeof(upper_list) > 2
			&& 3 last points make a LEFT turn */
		while( flag &&  drct>=-0.00001){
			/* DELETE the middle point */
			hull_idxs[tracker-2] = hull_idxs[tracker-1];
			hull_idxs[tracker-1] = 0;
			tracker--;

#if GRAHAM_ANIMATION
			if (display) {
				convex_hull_update(result, hull_idxs, tracker);
				jarvis_march_anim(window, result, 0);
				usleep(TIME_STEP);
				// if (bov_window_should_close(window))
				// 	goto end_of_jarvis;
			}
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
		if (display) {
			convex_hull_update(result, hull_idxs, tracker);
			jarvis_march_anim(window, result, 0);
			usleep(TIME_STEP);
			// if (bov_window_should_close(window))
			// 	goto end_of_jarvis;
		}
#endif

		flag = 1;
		drct = direction(coord[hull_idxs[tracker-3]],
						 coord[hull_idxs[tracker-2]],
						 coord[hull_idxs[tracker-1]]);

		/* Step 5.2 : WHILE sizeof(upper_list) > 2
			&& 3 last points make a LEFT turn */
		while(flag && drct >=-0.00001){
			// DELETE the middle point
			hull_idxs[tracker-2] = hull_idxs[tracker-1];
			hull_idxs[tracker-1] = 0;
			tracker--;

#if GRAHAM_ANIMATION
			if (display) {
				convex_hull_update(result, hull_idxs, tracker);
				jarvis_march_anim(window, result, 0);
				usleep(TIME_STEP);
				// if (bov_window_should_close(window))
				// 	goto end_of_jarvis;
			}
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
	clock_t t1 = clock();

	result->method = "Graham Scan";
	result->time = (double) (t1-t0) / CLOCKS_PER_SEC;
	convex_hull_update(result, hull_idxs, tracker-1);

	/* Termination */
	free(sorted);
	free(hull_idxs);
#if GRAHAM_ANIMATION
	if (display) {
		sleep(2);
		result->display = 0;
		bov_points_delete(result->coordDraw);
		bov_order_delete(result->hullDraw);
		bov_window_delete(window);
	}
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

void quick_hull_anim(bov_window_t *window, struct convex_hull_t *hull, int* actual_hull, int actual_size)
{

	hull->nHull_GL = (GLsizei) hull->nHull;
	bov_order_t* hullDraw = bov_order_update(hull->hullDraw,
										(GLuint*) actual_hull,
										(GLsizei) actual_size);
	bov_points_set_color(hull->coordDraw, (GLfloat[4]) BLACK);
	bov_points_draw(window, hull->coordDraw, 0, hull->nPoints_GL);
	bov_points_set_color(hull->coordDraw, (GLfloat[4]) CHARTREUSE);
	bov_line_loop_draw_with_order(window, hull->coordDraw, hullDraw, 0, actual_size);
	bov_window_update(window);
	sleep(1);
}

int quick_hull_rec(int* S, int size_S, int V_i, int V_j, float coord[][2], int* return_hull, int flag_left, struct convex_hull_t* anim_hull, bov_window_t* window){
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

		#if QUICKHULL_ANIMATION
			anim_hull->hull_idxs[anim_hull->nHull] = V;
			anim_hull->nHull++;
		#endif

		int* V1 = calloc(size_I+2, sizeof(int));
		int* V2 = calloc(size_J+2, sizeof(int));

		int length_V1 = quick_hull_rec(I, size_I, V_i, V, coord, V1, 1, anim_hull, window);
		int length_V2 = quick_hull_rec(J, size_J, V, V_j, coord, V2, 1, anim_hull, window);

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

		#if QUICKHULL_ANIMATION
			anim_hull->hull_idxs[anim_hull->nHull] = V;
			anim_hull->nHull++;
		#endif

		int* V1 = calloc(size_I+2, sizeof(int));
		int* V2 = calloc(size_J+2, sizeof(int));

		int length_V1 = quick_hull_rec(I, size_I, V_i, V, coord, V1, 0, anim_hull, window);
		int length_V2 = quick_hull_rec(J, size_J, V, V_j, coord, V2, 0, anim_hull, window);

		for(int i=0; i<length_V1; i++){
			return_hull[i] = V1[i];
		}
		for(int j=0; j<length_V2; j++){
			return_hull[length_V1+j] = V2[j];
		}
		return length_V1+length_V2;
	}
}

//INIT quickhull
struct convex_hull_t* quickhull(int nPoints, float coord[][2], int display){

	bov_window_t* window;
	if (display) {
		window = bov_window_new(800, 800, "Quick Hull Algorithm");
		bov_window_set_color(window, (GLfloat[]){0.9f, 0.85f, 0.8f, 1.0f});
	}
	struct convex_hull_t* result = malloc(sizeof(struct convex_hull_t));
	convex_hull_init(result, 0, nPoints, coord, QUICKHULL_ANIMATION);

	result->method = "QuickHull";
	result->time = 0;

	int *indexHull = malloc(sizeof(int)*nPoints);
	int *indexHullSecond = malloc(sizeof(int)*nPoints);

	int* S = calloc(nPoints, sizeof(int));
	int* S_left = calloc(nPoints, sizeof(int));
	int* S_right = calloc(nPoints, sizeof(int));
	int left_tracker = 0; int right_tracker = 0;


	argsort(nPoints, coord, 1, S);

	for(int i=1; i<nPoints-1; i++){
		if(direction(coord[S[0]], coord[S[nPoints-1]], coord[S[i]])>0){
			S_left[left_tracker] = S[i];
			left_tracker++;
		} else {
			S_right[right_tracker] = S[i];
			right_tracker++;
		}
	}

	struct convex_hull_t *anim_Hull = malloc(sizeof(struct convex_hull_t));
	struct convex_hull_t *anim_Hull_second = malloc(sizeof(struct convex_hull_t));

	#if QUICKHULL_ANIMATION
		convex_hull_init(anim_Hull, 0, nPoints, coord, QUICKHULL_ANIMATION);
		convex_hull_init(anim_Hull_second, 0, nPoints, coord, QUICKHULL_ANIMATION);
		anim_Hull->hull_idxs[0] = S[0]; anim_Hull->hull_idxs[1] = S[nPoints-1];
		anim_Hull->nHull = 2;
		anim_Hull_second->hull_idxs[0] = S[0]; anim_Hull_second->hull_idxs[1] = S[nPoints-1];
		anim_Hull_second->nHull = 2;
	#endif

	int nHull = quick_hull_rec(S_left, left_tracker, S[0], S[nPoints-1], coord, indexHull, 1, anim_Hull, window);
	int nHullSecond = quick_hull_rec(S_right, right_tracker, S[0], S[nPoints-1], coord, indexHullSecond, 0, anim_Hull_second, window);

	int precedent = -1;
	int* indexHullTotal = calloc(nHull+nHullSecond, sizeof(int));
	int hull_tracker = 0;
	for (int i=0; i<nHull-1; i++) {
		if(indexHull[i]!=precedent){
			indexHullTotal[hull_tracker] = indexHull[i];
			hull_tracker++;
		}
		precedent = indexHull[i];
	}
	for (int i=nHullSecond-1; i>0; i--) {
		if(indexHullSecond[i]!=precedent){
			indexHullTotal[hull_tracker] = indexHullSecond[i];
			hull_tracker++;
		}
		precedent = indexHullSecond[i];
	}

	float (*coordHull)[2] = malloc(sizeof(coordHull[0])*(hull_tracker));
	for (int i=0; i<hull_tracker; i++) {
		coordHull[i][0] = coord[indexHullTotal[i]][0];
		coordHull[i][1] = coord[indexHullTotal[i]][1];
	}
	convex_hull_update(result, indexHullTotal, hull_tracker);

	#if QUICKHULL_ANIMATION

	int* currentHull = calloc(hull_tracker, sizeof(int));
	int* temporelHull = calloc(hull_tracker, sizeof(int));
	int* indexs = calloc(hull_tracker, sizeof(int));
	int* anim_hull_total = calloc(hull_tracker, sizeof(int));
	int size_current = hull_tracker; int size_indexs=0;

	for(int m=0; m<anim_Hull_second->nHull; m++){
		anim_hull_total[m] = anim_Hull_second->hull_idxs[m];
	}
	for(int m=2; m<anim_Hull->nHull; m++){
		anim_hull_total[m+anim_Hull_second->nHull-2] = anim_Hull->hull_idxs[m];
	}


	for(int i=1; i<anim_Hull_second->nHull + anim_Hull->nHull-1; i++){
		size_indexs = 0;
		for(int p=0; p<hull_tracker; p++){
			currentHull[p] = indexHullTotal[p];
			size_current = hull_tracker;
		}
		for(int z = 0; z <i; z++){
			temporelHull[z] = anim_hull_total[z];
		}
		for(int j=0; j < hull_tracker; j++){
			int find = argfind(i, temporelHull, indexHullTotal[j]);
			if(find != -1){
				indexs[size_indexs] = argfind(hull_tracker, indexHullTotal, temporelHull[find]);
				size_indexs++;
			}
		}
		for(int z = 0; z <size_indexs; z++){
			currentHull[z] = indexHullTotal[indexs[z]];
		}

		quick_hull_anim(window, anim_Hull_second, currentHull, i);
	}
		if (display) {
			result->display = 0;
			bov_points_delete(result->coordDraw);
			bov_order_delete(result->hullDraw);
		}


	#endif
	bov_window_delete(window);
	free(anim_Hull);
	free(anim_Hull_second);
	return result;
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


struct convex_hull_t* chan_(int nPoints, float coord[][2], int display)
{
	int mPoints = 40;
	/* ------------------------------------------
		GRAHAM'S PARTITION OF THE SET OF POINTS
	--------------------------------------------- */

#if CHAN_ANIMATION
	bov_window_t* window;
	if (display) {
		window = bov_window_new(800, 800, "Chan Algorithm");
		bov_window_set_color(window, (GLfloat[]){0.9f, 0.85f, 0.8f, 1.0f});
	}
#endif

	/* Initialisation */
	// Display for Each Subsets
	struct convex_hull_t *result = malloc(sizeof(struct convex_hull_t));
	convex_hull_init(result, 0, nPoints, coord, CHAN_ANIMATION);

	clock_t t0 = clock();
	int count = 0;
	int countMax;
	int mSets,mLastPoints;

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
		myHulls[i] = *graham_scan(mPoints, coord_, 0);
		myHulls[i].Start = i*mPoints;
		countMax = countMax + myHulls[i].nHull;
	}

	for (int j=0; j<mLastPoints; j++) {
		coord_[j][0] = coord[mPoints*(mSets-1)+j][0];
		coord_[j][1] = coord[mPoints*(mSets-1)+j][1];
	}
	myHulls[mSets-1] = *graham_scan(mLastPoints, coord_, 0);
	myHulls[mSets-1].Start = (mSets-1)*mPoints;
	countMax = countMax + myHulls[mSets-1].nHull;

	int *hull_idxs = calloc(countMax, sizeof(int));

#if CHAN_ANIMATION
	if (display) {
		for (int i=0; i<mSets; i++)
			convex_hull_display_init(&myHulls[i], i%MAXCOLORS);
	}
#endif

#if CHAN_PRESENTATION
	if (display) {
		bov_points_draw(window, result->coordDraw, 0, result->nPoints_GL);
		bov_window_update(window);
		for (int i=0; i<mSets; i++) {
			bov_points_draw(window, result->coordDraw, 0, result->nPoints_GL);
			bov_points_draw(window, myHulls[i].coordDraw, 0, myHulls[i].nPoints_GL);
			bov_line_loop_draw_with_order(window, myHulls[i].coordDraw, myHulls[i].hullDraw, 0, myHulls[i].nHull_GL);
			bov_window_update(window);

			sleep(1);
			// clock_t t0 = clock(), t1 = clock();
			// while ((double) (t1-t0) / CLOCKS_PER_SEC < 2.0)
			// 	t1 = clock();
		}
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
		if (display) {
			hull_idxs[count] = next;
			convex_hull_update(result, hull_idxs, count+1);

			for (int i=0; i<mSets; i++)
				bov_line_loop_draw_with_order(window, myHulls[i].coordDraw, myHulls[i].hullDraw, 0, myHulls[i].nHull_GL);
			bov_line_strip_draw_with_order(window, result->coordDraw, result->hullDraw, 0, result->nHull_GL);
			bov_window_update(window);
			usleep(TIME_STEP);

			if (bov_window_should_close(window))
				goto end_of_march;
		}
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
			if (display) {
				// convex_hull_partial_update(result, &mySetNext, count, count+1, count+1);
				hull_idxs[count] = mySetNext;
				convex_hull_update(result, hull_idxs, count+1);

				for (int i=0; i<mSets; i++)
					bov_line_loop_draw_with_order(window, myHulls[i].coordDraw, myHulls[i].hullDraw, 0, myHulls[i].nHull_GL);
				bov_line_strip_draw_with_order(window, result->coordDraw, result->hullDraw, 0, result->nHull_GL);
				bov_window_update(window);
				usleep(TIME_STEP);
			}
#endif

			drct = direction(coord[curr], coord[mySetNext], coord[next]);
			if (drct<0.0)
				next = mySetNext;
		}
	}
	clock_t t1 = clock();
	convex_hull_update(result, hull_idxs, count);

	result->method = "Chan";
	result->time = (double) (t1-t0) / CLOCKS_PER_SEC;

	convex_hull_update(result, hull_idxs, count);

end_of_march:
	free(hull_idxs);
#if CHAN_ANIMATION
	if (display) {
		for (int i=0; i<mSets; i++)
			bov_line_loop_draw_with_order(window, myHulls[i].coordDraw, myHulls[i].hullDraw, 0, myHulls[i].nHull_GL);
		bov_line_strip_draw_with_order(window, result->coordDraw, result->hullDraw, 0, result->nHull_GL);
		bov_window_update(window);
		sleep(2);
		result->display = 0;
		bov_points_delete(result->coordDraw);
		bov_order_delete(result->hullDraw);
		bov_window_delete(window);
	}
#endif
	return result;
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

		nPoints = hull->nHull+1;
		int *hull_idxs = calloc(nPoints, sizeof(int));
		coord[0][0] = point[0]; coord[0][1] = point[1];
		for (int i=0; i<hull->nHull; i++) {
			coord[i+1][0] = hull->coord[hull->hull_idxs[i]][0];
			coord[i+1][1] = hull->coord[hull->hull_idxs[i]][1];
		}

		struct convex_hull_t *hull_intermediate = graham_scan(nPoints, coord, 0);
		hull_new->nHull = hull_intermediate->nHull;
		for (int i=0; i<hull_new->nHull; i++) {
			if (hull_intermediate->hull_idxs[i]==0)
				hull_new->hull_idxs[i] = hull_new->nPoints-1;
			else
				hull_new->hull_idxs[i] = hull->hull_idxs[hull_intermediate->hull_idxs[i]-1];
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

		convex_hull_init(hull_new, 0, nPoints, coord, 1);

		int idh = argfind(hull->nHull, hull->hull_idxs, idx);
		if (idh!=-1) {
			/* The considered 'point' is in the hull
				→ Delete it from hull_idxs
				→ Does the hull need to be changed ?
			*/

			struct convex_hull_t *hull_recomp = chan_(nPoints, coord, 0);
			hull->hull_idxs = hull_recomp->hull_idxs;
			hull->nHull = hull_recomp->nHull;
			// quickhull with the 2 POINTS
			// float (*hullcoord)[2] = calloc(hull->nHull, sizeof(coord[0]));
			// for(int i=0; i< hull->nHull; i++){
			// 	hullcoord[i][0] = hull->coord[hull->hull_idxs[i]][0];
			// 	hullcoord[i][1] = hull->coord[hull->hull_idxs[i]][1];
			// }
			// int min = argmin(hull->nHull, hullcoord, 1);
			// int max = argmax(hull->nHull, hullcoord, 1);
			//
			// if(direction(hullcoord[min], hullcoord[max], point)<0){
			// 	int V1 = hull_idxs[idh+1];
			// 	int V2 = hull_idxs[idh-1];
			// 	int* S = calloc(hull->nPoints, sizeof(int));
			// 	int size_S = 0;
			// 	for(int j = 0; j < hull->nPoints; j++){
			// 		if(direction(hull->coord[V1], hull->coord[V2], hull->coord[j])<0){
			// 			S[size_S] = j;
			// 			size_S++;
			// 		}
			// 	}
			//
			// 	bov_window_t* window;
			// 	struct convex_hull_t* anim_hull;
			// 	int* return_hull = calloc(hull->nPoints, sizeof(int));
			// 	int n_add_hull = quick_hull_rec(S, size_S, V2, V1, coord, return_hull, 0, anim_hull, window);
			// 	printf("idh = %d, n_add_hull = %d \n", idh, n_add_hull);
			// 	for(int k = hull->nHull-1; k>=idh+n_add_hull; k--){
			// 		printf("%d, ", k-n_add_hull);
			// 		hull->hull_idxs[k] = hull->hull_idxs[k-n_add_hull];
			// 	}
			// 	printf("HERE \n");
			// 	for(int k=idh; k<idh+n_add_hull; k++){
			// 		hull->hull_idxs[k] = return_hull[k-idh];
			// 	}
			// 	hull->nHull = hull->nHull + n_add_hull;
			//
			// } else {
			//
			// }

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
