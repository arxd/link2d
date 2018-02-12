
#include "util.c"
#include "gl.c"
#include "shaders.h"

const char *gl_name = "Link2d";

void gl_init(void)
{
	DEBUG("GL INIT");
	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(0.498, 0.624 , 0.682, 1.0);
	
}


void gl_frame(void)
{
	char key;
	switch(key = key_pop()) {
		case 0: break;
		default: DEBUG("%7d:'%c'", GW.frame, key); break;
	}
	glClear(GL_COLOR_BUFFER_BIT);
	draw_color(0.2, 0.3, 0.5, 1.0);
	glLineWidth(4.0);
	draw_line_strip(3, (GLfloat[]){0.0, 0.0, -0.5, -0.5, 1.0, 0.0});

	
	
}



