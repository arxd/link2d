//~ #include <math.h>
#include <malloc.h>
#include "logging.c"
#include "gl.c"
#include "math.c"







const char *gl_name = "I Love You";
V2 heart_pts[] = {
	{0.0, 0.0},	
	{0.0, -7.0}, {3.5, -4.0}, {6.0, 1.0}, {6.0, 4.0}, {5.0, 6.0}, {3.0, 6.0}, {0.0, 3.0},
	{-3.0, 6.0}, {-5.0, 6.0}, {-6.0, 4.0}, {-6.0, 1.0}, {-3.5, -4.0}, {0.0, -7.0}};

V2 M_pts[] = {
	{-6.0, -6.0},{-6.0, 6.0},{0.0, 0.0},{6.0, 6.0},{6.0, -6.0}};
	
V2 A0_pts[] = {
	{-6.0, -6.0},{-1.0, 6.0},{1.0, 6.0},{6.0, -6.0}};
V2 A1_pts[] = {
	{-3.0, 0.0},{4.0, 0.0}};

float heart_pathlen;

	
typedef struct s_Path Path;
struct s_Path {
	int npts;
	V2 *pts;
};

void path_init(Path *self, V2 p0)
{
	memset(self, 0, sizeof(Path));
	self->pts = (V2*)malloc(sizeof(V2) * 1024);
	self->npts = 1;
	self->pts[0] = p0;
}

void path_free(Path *self)
{
	if (self->pts)
		free(self->pts);
	memset(self, 0, sizeof(Path));	
}

float path_length(Path *self)
{
	float len = 0.0;
	if (self->npts < 2)
		return 0.0;
	for (int i=1; i < self->npts; ++i)
		len += v2dist(self->pts[i], self->pts[-1]);
	return len;
}

void path_append(Path *self, V2 pt)
{
	self->npts += 1;
	self->pts = realloc(self->pts, sizeof(V2)*self->npts);
	self->pts[self->npts-1] = pt;
}

void path_join(Path *self, Path *other)
{
	self->npts += other->npts;
	self->pts = realloc(self->pts, sizeof(V2)*self->npts);
	memcpy(&self->pts[self->npts - other->npts], other->pts, sizeof(V2)*other->npts);
}

void path_copy(Path *self, Path *other)
{
	self->npts = other->npts;
	self->pts = realloc(self->pts, sizeof(V2)*self->npts);
	memcpy(self->pts, other->pts, sieof(V2)*other->npts);
}

void path_reverse(Path *self)
{
	for (int i=0; i < self->npts / 2; ++i) {
		V2 tmp = self->pts[i];
		self->pts[i] = self->pts[self->npts-1-i];
		self->pts[self->npts-1-i] = tmp;
	}
}


typedef struct s_Meander Meander;
struct s_Meander {
	Path path;
	V1 d0;
	V1 d1;
	V1 d2;
	V1 d3;
	V1 d4;
};


void meander_next(Meander *self)
{
	path_append(&self->path, v2add(self->path.pts[self->path.npts-1], v2mult(v2dir(self->d0), dt)));
	
	
	
}


	
#define NHEARTS 8*7
typedef struct s_Heart Heart;
struct s_Heart {
	V2 xy;
	V2 v;
	V1 a;
	V1 size;
	V2 target;
};

Heart hearts[NHEARTS];



void heart_to(int hid, V2 xy)
{
	hearts[hid].target = xy;
	hearts[hid].v = v2mult(v2sub(hearts[hid].target, hearts[hid].xy), 1.0/(rand()%60 + 70));	
}

void hearts_to_path(int id0, int nhts, int npts , V2 *pts)
{
	float pathlen = 0.0;
	int i=0;
	for (; i < npts-1; ++i)
		pathlen += v2dist(pts[i], pts[i+1]);
	
	double off = 0.0;
	i = 0;
	for (int h = id0; h < id0+nhts; ++h) {
		heart_to(h, v2add(pts[i], v2mult(v2norm(v2sub(pts[i+1], pts[i])), off)));
		off += pathlen/nhts;
		while (off > v2dist(pts[i], pts[i+1])) {
			off -= v2dist(pts[i], pts[i+1]);
			i ++;
		}
	}
}

void hearts_random(float dx, float dy)
{
	for (int h=0; h < NHEARTS; ++h) 
		heart_to(h, v2(dx*(rand()*2.0/RAND_MAX-1.0),dy*(rand()*2.0/RAND_MAX-1.0)));
}



void gl_init(void)
{
	INFO("GL INIT");
	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(0.498, 0.624 , 0.682, 1.0);
	
	//~ heart_pathlen = 0.0;
	//~ for (int i=0; i < 12; ++i) {
		//~ heart_pathlen += seglen(X0, Y0, X1, Y1);
		
	//~ }
	INFO("PATHLEN: %f", heart_pathlen);
	GW.zoomx = GW.zoomy = 20.0;
	
	for (int h =0; h < NHEARTS; ++h) {
		hearts[h].xy = v2(0.0, 0.0);
		hearts[h].v = v2(0.0, 0.0);
		hearts[h].a = 0.0;
		hearts[h].size = 1.0/16.0;
	}
	
	hearts_random(7.0, 7.0);//(0, 40, 13, &heart_pts[1]);
	
	
	
	
	
}




void update_hearts(void)
{
	for (int h = 0; h  < NHEARTS; ++h) {
		hearts[h].xy = v2add(hearts[h].xy, hearts[h].v);
		if (v2dist(hearts[h].xy, hearts[h].target) < 0.1) {
			hearts[h].xy = hearts[h].target;
			hearts[h].v = v2(0.0, 0.0);
		}
	}
}

void draw_hearts(void)
{
	for (int h = 0; h  < NHEARTS; ++h)
		draw_polygon(hearts[h].xy.xy, (float[2]){hearts[h].size, hearts[h].size},hearts[h].a, 14,(GLfloat*)heart_pts);
}

//~ void draw_hearts(int num, float scale)
//~ {
	//~ float seg = 0.0, total=0.0;
	//~ int i = 0;
	
	//~ while (num--) {
		//~ draw_polygon((float[2]){X(seg), Y(seg)}, (float[2]){scale, scale}, 0.0, 14, heart);
		//~ seg += heart_pathlen / num;
		//~ while (num && seg > seglen(X0, Y0, X1, Y1)) {
			
			//~ seg -= seglen(X0, Y0, X1, Y1);
			//~ ++i;
		//~ }
	//~ }
	
	
//~ }

int state = 0;

int gl_frame(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	draw_color(0.2, 0.3, 0.5, 1.0);
	glLineWidth(4.0);
	//~ draw_line_strip(v2(0.0,0.0).xy, v2(1.0, 1.0).xy, 0.0, 4, (GLfloat*)&A0_pts);
	//~ draw_line_strip(v2(0.0,0.0).xy, v2(1.0, 1.0).xy, 0.0, 2, (GLfloat*)&A1_pts);
	//~ draw_polygon((float[2]){0.0, 0.0}, (float[2]){1.0, 1.0}, 0.0, );
	//~ draw_polygon((float[2]){10.0, 0.0}, (float[2]){1.0, 1.0}, 0.0, 14, heart);
	//~ draw_polygon((float[2]){0.0, 10.0}, (float[2]){1.0, 1.0}, 90.0, 14, heart);

	//~ if (GW.frame % 11 == 0 && state%2) {
		//~ hearts_random(7.0*5.0, 7.0*5.0);//(0, 40, 13, &heart_pts[1]);
	//~ }
	
	//~ if (GW.frame % (59*3) == 0) {
		//~ state = (state+1)%8;
		//~ if (state%8 == 0 || state%8 == 4) {
			//~ hearts_to_path(0, NHEARTS, 13, &heart_pts[1]);
		//~ } else if (state % 8 == 2) {
			//~ hearts_to_path(0, NHEARTS, 5, M_pts);
		//~ } else if (state % 8 == 6) {
			//~ hearts_to_path(0, NHEARTS/7*6, 4, A0_pts);
			//~ hearts_to_path(NHEARTS/7*6, NHEARTS/7, 2, A1_pts);
		//~ }
		
		
		
	//~ }
	
	

	//~ draw_color(1.0, 0.3, 0.5, 1.0);
	//~ update_hearts();
	//~ draw_hearts();
	
	if (GW.alpha & KALPHA_A)
		GW.camx -= 10.0;
	if (GW.alpha & KALPHA_D) 
		GW.camx += 10.0;
	if (GW.alpha & KALPHA_S) 
		GW.camy -= 10.0;
	if (GW.alpha & KALPHA_W) 
		GW.camy += 10.0;
	//~ if (input->alpha & KALPHA_Q) 
		//~ GW.angle += 0.06;
	//~ if (input->alpha & KALPHA_E) 
		//~ GW.angle -= 0.06;
	if (GW.alpha & KALPHA_R) 
		GW.zoomx *= 1.1;
	if (GW.alpha & KALPHA_F) 
		GW.zoomx /= 1.1;
	GW.zoomy = GW.zoomx;
	//~ INFO("%.1f, %.1f", GW.camx, GW.camy);
	return !(GW.cmd & KCMD_ESCAPE);
}

int main_init(int argc, char *argv[])
{
	return 0;
}


