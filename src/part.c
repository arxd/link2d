#ifndef PART_C
#define PART_C

#include "math.c"
#include "nurbs.c"

typedef struct s_PartClass PartClass;
typedef struct s_PartInst PartInst;
typedef struct s_PartHole PartHole;
typedef struct s_PartSlot PartSlot;
typedef struct s_Pin Pin;

struct s_PartHole {
	NurbsLoop *outline;
	PartHole *next, *prev;
};

struct s_PartSlot {
	NurbsCurve *line;
	PartSlot *next, *prev;
};

struct s_Pin {
	V2 xy;
	Pin *next, *prev;
	
};

struct s_PartClass {
	NurbsLoop *outline;
	PartHole holes;
	PartSlot slots;
	Pin pins;
};


struct s_PartInst {
	V2 xy;
	V1 rotation;
};

#if __INCLUDE_LEVEL__ == 0 || defined(PIXIE_NOLIB)


#endif
#endif
