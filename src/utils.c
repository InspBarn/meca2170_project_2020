#include "utils.h"

// compute the distance between two points
float distance(const float x1[2], const float x2[2]) {
	return sqrt(pow(x1[0]-x2[0],2.) + pow(x1[1]-x2[1],2.));
}

//insert a point in an array
void insert(int nPoints, int *vec, int arg, int idx)
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

//find a point in an array
int find(int nPoints, float coord[][2], const float point[2])
{
	float eps = 5e-3, dst;
	for(int i=0; i<nPoints; i++)
		if (distance(coord[i], point) < eps)
			return i;
	return -1;
}

//find minimum of array and return the index
int argmin(int nPoints, float coord[][2], int axis)
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

//find maxmum of array and return the index
int argmax(int nPoints, float coord[][2], int axis)
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

//sort an array (with qsort) and return the indexs
void argsort(int nPoints, float coord[][2], int axis, int* argsorted_list)
{
	struct sort array[nPoints];
    for (int i = 0; i < nPoints; i++) {
		array[i].value = coord[i][axis];
		array[i].index = i;
	}
	// Sort objects array according to value maybe using 'qsort'
	qsort(array, nPoints, sizeof(array[0]), cmp);
	for (int i = 0; i < nPoints; i++)
		argsorted_list[i] = array[i].index;
	/*
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
	*/
}
//find a point in an array and return the index
int argfind(int nPoints, int *vec, int idx)
{
	for(int i=0; i<nPoints; i++){
		if(idx==vec[i]){
			return i;
		}
	}
	return -1;
}

//compute the area of a triangle -> used to determin if you turn clockwise or anti-clockwise
double direction(float x1[2], float x2[2], float x3[2])
{
	double area = (x1[0]*x2[1] - x2[0]*x1[1]) \
				- (x1[0]*x3[1] - x3[0]*x1[1]) \
				+ (x2[0]*x3[1] - x3[0]*x2[1]);
	return area;
}

//comparaison function used by argsort
int cmp(const void *a, const void *b)
{
    struct sort *a1 = (struct sort *)a;
    struct sort *a2 = (struct sort *)b;
    if ((*a1).value > (*a2).value)
        return -1;
    else if ((*a1).value < (*a2).value)
        return 1;
    else
        return 0;
}


/*
-------------------------------------------------
                   CONVEX HULL
-------------------------------------------------
*/

void convex_hull_init(struct convex_hull_t *hull, int start, int stop, float coord[][2], int display)
{
	hull->display = display;
	hull->Start = start;
	hull->nPoints = stop-start;

	hull->coord = (float(*)[2])malloc(sizeof(hull->coord[0])*hull->nPoints);
	hull->hull_idxs = (int*)malloc(sizeof(int)*hull->nPoints);

	for(int i=0; i<hull->nPoints; i++) {
		hull->coord[i][0] = coord[start+i][0];
		hull->coord[i][1] = coord[start+i][1];
	}

	hull->nHull = 1;
	hull->hull_idxs[0] = 0;
	// hull->nHull = hull_function(hull->nPoints, hull->coord, hull->hull_idxs);

	if (display){
		convex_hull_display_init(hull, -1);
	}

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
