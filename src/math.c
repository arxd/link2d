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
V1 v1norm(V1 mean, V1 stdev);
V1 v2dist(V2 v0, V2 v1);
V1 v2mag(V2 v0);
V1 v2ang(V2 v0);
V2 v2sub(V2 v0, V2 v1);
V2 v2add(V2 v0, V2 v1);
V2 v2norm(V2 v0);
V2 v2mult(V2 v0, V1 t);
V2 v2dir(V1 degrees);
V2 v2neg(V2 v0);

const char *v2str(V2 v0);


typedef V1 Analytic1(int n);
typedef V2 Analytic2(int n);


#if __INCLUDE_LEVEL__ == 0 || defined(PIXIE_NOLIB)

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

V1 v1norm(V1 mean, V1 stdev)
{
	static unsigned char off = 0;
	static V1 z0;
	
	off ++;
	if (off & 1 == 0)
		return z0;
	V1 r = stdev * sqrt(-2.0*log(rand()*1.0/RAND_MAX));
	V1 a = rand()*2.0*M_PI/RAND_MAX;
	z0 = r*cos(a) + mean;
	return r*sin(a) + mean;
}



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
V2 v2neg(V2 v0)
{
	return (V2){-v0.x, -v0.y};
}

V1 v2mag(V2 v0)
{
	return sqrt(v0.x*v0.x + v0.y*v0.y);
}

V1 v2ang(V2 v0)
{
	if (v0.x == 0.0 && v0.y == 0.0)
		return 360.0;
	return fmod(atan2(v0.y, v0.x)*180.0/M_PI+360.0, 360.0);
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

const char *v2str(V2 v0)
{
	static unsigned char idx = 0;
	static char buffer[256][32];
	idx = idx + 1;
	snprintf(buffer[idx], 32, "<%.1f, %.1f>", v0.x, v0.y);
	return buffer[idx];
}


#endif
#endif
