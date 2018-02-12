#ifndef MATH_C
#define MATH_C

typedef double V1;

typedef union u_V2 V2;

union u_V2 {
	V1 xy[2];
	struct {
		V1 x;
		V1 y;
	};
};

V2 v2(V1 x, V1 y);


typedef V1 Analytic1(int n);
typedef V2 Analytic2(int n);



#if __INCLUDE_LEVEL__ == 0 || defined(PIXIE_NOLIB)

V2 v2(V1 x, V1 y)
{
	return (V2){x, y};
}



#endif
#endif
