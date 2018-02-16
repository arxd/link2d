#ifndef MATH_C
#define MATH_C

typedef float V1;

typedef union u_V2 V2;

union u_V2 {
	V1 xy[2];
	struct {
		V1 x;
		V1 y;
	};
};

V2 v2(V1 x, V1 y);
V1 v2dist(V2 v0, V2 v1);
V1 v2mag(V2 v0);
V2 v2sub(V2 v0, V2 v1);
V2 v2add(V2 v0, V2 v1);
V2 v2norm(V2 v0);
V2 v2mult(V2 v0, V1 t);
V2 v2dir(V1 degrees);



typedef V1 Analytic1(int n);
typedef V2 Analytic2(int n);


#if __INCLUDE_LEVEL__ == 0 || defined(PIXIE_NOLIB)

#include <math.h>

V2 v2(V1 x, V1 y)
{
	return (V2){x, y};
}

V2 v2sub(V2 v0, V2 v1)
{
	return (V2){v0.x-v1.x, v0.y-v1.y};
}

V2 v2add(V2 v0, V2 v1)
{
	return (V2){v1.x + v0.x, v1.y + v0.y};
}

V1 v2mag(V2 v0)
{
	return sqrt(v0.x*v0.x + v0.y*v0.y);
}

V1 v2dist(V2 v0, V2 v1)
{
	return v2mag(v2sub(v0, v1));
}

V2 v2norm(V2 v0)
{
	return v2mult(v0, 1.0/v2mag(v0));
}

V2 v2mult(V2 v0, V1 t)
{
	return (V2){v0.x*t, v0.y*t};
}


V2 v2dir(V1 degrees)
{
	return (V2){cos(M_PI*degrees/180.0), sin(M_PI*degrees/180.0)};
}


#endif
#endif
