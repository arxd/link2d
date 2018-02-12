#ifndef GL_C
#define GL_C

#define GLFW_INCLUDE_ES2
#include <GLFW/glfw3.h>

typedef struct s_Window Window;
typedef struct s_Shader Shader;
typedef struct s_Texture Texture;
typedef struct s_FrameBuffer FrameBuffer;

struct s_Window {
	GLFWwindow *window;
	GLFWwindow *w_window;
	GLFWwindow *fs_window;
	int frame;
	
	int w, h;
	int x, y;
	
	int cx, cy;
	char input_chars[256];
	int input_i;
	int fs;
	
	GLfloat color[4];
	
	GLfloat camx, camy;
	GLfloat zoom;
	GLfloat vmat[3][3];

};

extern Window GW;

struct s_Shader {
	GLuint id;
	GLuint argc;
	GLuint args[16];
};

struct s_Texture {
	GLuint id;
	int w, h;
//	GLint internal_format;
	GLenum format;
	GLenum type;
	//~ void *data;
};

struct s_FrameBuffer {
	GLuint id;
	Texture tex;
};

int shader_init(Shader *self, const char *vert_src, const char *frag_src, char *args[]);
void shader_fini(Shader *self);
void shader_on_exit(int status, void *arg);

void clear(GLfloat r, GLfloat g, GLfloat b);
void tex_set(Texture *self, void * pixels);
void tex_on_exit(int status, void *arg);
void tex_fini(Texture *self);


void gl_error_check(void);

int framebuffer_init(FrameBuffer *self, int w, int h);
void framebuffer_fini(FrameBuffer *self);
void framebuffer_on_exit(int status, void *arg);

void draw_color(float r, float g, float b, float a);
void draw_line_strip(int npts, GLfloat *pts);
void draw_line_loop(int npts, GLfloat *pts);
void draw_lines(int npts, GLfloat *pts);

double time(void);

char key_pop(void);

#if __INCLUDE_LEVEL__ == 0

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <math.h>
#include <string.h>

#include "util.c"
#include "shaders.h"

Window GW;

extern void gl_frame(void);
extern void gl_init(void);
extern const char *gl_name;


void clear(GLfloat r, GLfloat g, GLfloat b)
{
	glClearColor(r, g , b, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
}

void gl_error_check(void)
{
	int err;
	if ( (err=glGetError()) )
		ABORT(3, "GL Error %d", err);
}

void tex_set(Texture *self, void * pixels )
{
	if (!self->id) {// create a new texture
		glGenTextures(1, &self->id);
		DEBUG("Create texture %d", self->id);
		glBindTexture(GL_TEXTURE_2D, self->id);
		glTexImage2D(GL_TEXTURE_2D, 0, self->format, self->w, self->h, 0, self->format, self->type,  pixels);
		gl_error_check();

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	} else {
		glBindTexture(GL_TEXTURE_2D, self->id);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, self->w, self->h, self->format, self->type,  pixels);
		
		
	}//~ glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // only power of 2 textures can be wrapped 
		//~ glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // only power of 2 textures can be wrapped 

	
}

void tex_fini(Texture *self)
{
	if (self->id) {
		DEBUG("Unbind texture %d", self->id);
		glDeleteTextures(1, &self->id);
	}
	self->id = 0;
}

void tex_on_exit(int status, void *arg)
{
	tex_fini((Texture*)arg);
}


int glsl_check(
	void (*get_param)(GLuint,  GLenum,  GLint *),
	void (*get_log)(GLuint,  GLsizei,  GLsizei *,  GLchar *),
	GLuint prog,
	GLuint param)
{
	GLint status;
	get_param(prog, param, &status);
	if (!status) {
		GLint len = 0;
		get_param(prog, GL_INFO_LOG_LENGTH, &len);
		if ( len > 0 ) {
			char* log = alloca(sizeof(char) * len);
			get_log(prog, len, NULL, log);
			printf("Compile Error : %d : %s\n", prog, log);
		}
		return -1;
	}
	return 0;
}

void shader_fini(Shader *self)
{
	if (self->id){
		DEBUG("Program deleted %d", self->id);
		glDeleteProgram(self->id);
	}
	self->id = 0;
}

void shader_on_exit(int status, void *arg)
{
	shader_fini((Shader*)arg);
}

int shader_init(Shader *self, const char *vert_src, const char *frag_src, char *args[])
{
	self->id = 0;
	self->argc = -1;
	while (args[++self->argc]);
	
	//~ Shader *prog = malloc(sizeof(Shader) + sizeof(GLuint)*argc);
	//~ prog->argc = argc;
	ASSERT (self->argc < 16);

	GLuint vert_shader, frag_shader;
	// Compile Vertex Shader
	vert_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vert_shader, 1, &vert_src, NULL);
	glCompileShader(vert_shader);
	if(glsl_check(glGetShaderiv,  glGetShaderInfoLog, vert_shader, GL_COMPILE_STATUS))
		return 0;
	
	// Compile Fragment Shader
	frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(frag_shader, 1, &frag_src, NULL);
	glCompileShader(frag_shader);
	if(glsl_check(glGetShaderiv,  glGetShaderInfoLog, frag_shader, GL_COMPILE_STATUS)) {
		glDeleteShader(vert_shader);
		return 0;
	}
	
	// Link the program
	self->id = glCreateProgram();
	glAttachShader(self->id, vert_shader);
	glAttachShader(self->id, frag_shader);
	glLinkProgram(self->id);
	if(glsl_check(glGetProgramiv,  glGetProgramInfoLog, self->id, GL_LINK_STATUS)) {
		glDeleteShader(vert_shader);
		glDeleteShader(frag_shader);
		shader_fini(self);
		return 0;
	}	
	// figure out program locations from arguments
	for (int a =0; a < self->argc; ++a) {
		switch (args[a][0]) {
			case 'a':
				self->args[a] = glGetAttribLocation(self->id, args[a]);
				//~ glEnableVertexAttribArray(prog->args[a]);
			break;
			case 'u': self->args[a] = glGetUniformLocation(self->id, args[a]); break;
			default: printf("WARNING: Must be attribute or uniform: %s",args[a]); break;
		}
	}
	
	glDeleteShader(vert_shader); // flagged for deletion when program is deleted
	glDeleteShader(frag_shader); // flagged for deletion when program is deleted
	//~ glValidateProgram(prog->id);
	//~ if(glsl_check(glGetProgramiv,  glGetProgramInfoLog, prog->id, GL_VALIDATE_STATUS))
		//~ return NULL;

	DEBUG("Program compiled %d", self->id);
	return 1;
}

int framebuffer_init(FrameBuffer *self, int w, int h)
{
	if (self->id) {// releaase the old framebuffer
		glDeleteFramebuffers(1, &self->id);
	} else { // this is the first time we are called
		DEBUG("Create New Texture for framebuffer");
		self->tex.w = w;
		self->tex.h = h;
		self->tex.format = GL_RGB;
		self->tex.type = GL_UNSIGNED_BYTE;
		tex_set(&self->tex, NULL);
	}
	glGenFramebuffers(1, &self->id);
	glBindFramebuffer(GL_FRAMEBUFFER, self->id);
	DEBUG("New FrameBuffer %d", self->id);
	//~ glGenTextures(1, &self->txid);
	//~ glBindTexture(GL_TEXTURE_2D, self->txid);
	//~ glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, self->w, self->h, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		//~ printf("glTexImage2D : %d\n", glGetError());

	//~ glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//~ glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//~ glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // only power of 2 textures can be wrapped 
	//~ glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // only power of 2 textures can be wrapped 

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, self->tex.id, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		ABORT(1, "Framebuffer not ready %d", glCheckFramebufferStatus(GL_FRAMEBUFFER));
	glBindFramebuffer(GL_FRAMEBUFFER, 0); // put the default framebuffer back
	return 1;
}

void framebuffer_fini(FrameBuffer *self)
{
	tex_fini(&self->tex);
	if (self->id) {
		DEBUG("Delete fb %d", self->id);
		glDeleteFramebuffers(1, &self->id);
	}
	memset(self, 0, sizeof(FrameBuffer));
}

void framebuffer_on_exit(int status, void *arg)
{
	framebuffer_fini((FrameBuffer*)arg);
}


//~ void gl_context_change(void);

//~ Window ;

double time(void)
{
	return glfwGetTime ();
}



//~ void win_size(int *w, int *h)
//~ {
	//~ *w = gw.w;
	//~ *h = gw.h;
	//~ glfwGetFramebufferSize(gw.window, w, h);
//~ }
char key_pop(void)
{
	if (GW.input_i)
		return GW.input_chars[--GW.input_i];
}

double win_time(void)
{
	return glfwGetTime();
}

//~ void win_update(void)
//~ {
	//~ glfwSwapBuffers(gw.window);
	//~ glfwPollEvents();
	//~ if (gw.input_i)
		//~ input->getchar = gw.input_chars[--gw.input_i];
	//~ if (glfwWindowShouldClose(gw.window))
		//~ input->status |= STATUS_CLOSE;
	
	//~ int x,y;
	//~ static int f = 0;
	//~ ++f;
	//~ glfwGetWindowPos	(gw.window, &x, &y);
	//~ if(! (f%30))
		//~ printf("%d, %d\n", x, y);

//~ }


void set_window(GLFWwindow *w);

GLFWmonitor* cur_monitor(void)
{
	int count, xpos, ypos;
	GLFWmonitor** monitors = glfwGetMonitors(&count);
	GLFWmonitor *monitor =glfwGetPrimaryMonitor() ;
	glfwGetWindowPos(GW.window, &xpos, &ypos);
	for (int m=0; m < count; ++m) {
		int mx, my;
		glfwGetMonitorPos(monitors[m], &mx, &my);
		const GLFWvidmode* mode = glfwGetVideoMode(monitors[m]);
		DEBUG("MONITOR %s: %dx%d (%d, %d) %d", glfwGetMonitorName(monitors[m]), 
			mode->width,mode->height, mx,my, mode->refreshRate);
		int nmodes;
		const GLFWvidmode* modes = glfwGetVideoModes(monitors[m], &nmodes);
		for (int d=0; d < nmodes; ++d)
			DEBUG ("\t%dx%d %d", modes[d].width, modes[d].height, modes[d].refreshRate);
		
		if (xpos > mx && ypos > my && xpos < mx+mode->width && ypos < my+mode->height)
			monitor = monitors[m];
	}
	return monitor;
}

//~ void do_fullscreen2(void)
//~ {
	//~ static int xpos, ypos, width, height;

	//~ if (gw.fs) {
		//~ glfwSetWindowMonitor(gw.window, NULL, xpos, ypos, width, height, 0);
	//~ } else {
		//~ glfwGetWindowPos(gw.window, &xpos, &ypos);
		//~ glfwGetWindowSize(gw.window, &width, &height);
		//~ // figure out which monitor we are on
		//~ GLFWmonitor *monitor = cur_monitor();
		
		//~ const GLFWvidmode* mode = glfwGetVideoMode(monitor);
		//~ DEBUG("FS ON %s: %dx%d", glfwGetMonitorName(monitor), mode->width, mode->height);
		//~ glfwSetWindowMonitor(gw.window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
	//~ }
	//~ gw.fs = !gw.fs;
	
//~ }

void do_fullscreen(void)
{
	if (!GW.fs_window) { // we need to make a fullscreen window
		GLFWmonitor *monitor =cur_monitor();
		int count;
		//~ GLFWmonitor** monitors = glfwGetMonitors(&count);
		//~ for (int m=0; m < count; ++m)
			//~ DEBUG("MONITOR %d: %s ", m, glfwGetMonitorName(monitors[m]));
		
		const GLFWvidmode* mode = glfwGetVideoMode(monitor);
		glfwWindowHint(GLFW_RED_BITS, mode->redBits);
		glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
		glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
		glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate); 
		DEBUG("Make fs window %dx%d %d", mode->width, mode->height, mode->refreshRate);
		GW.fs_window = glfwCreateWindow(mode->width, mode->height, "Glyphy FS", monitor, GW.w_window);
		if (!GW.fs_window) {
			fprintf(stderr, "Failed to create a fs window!\n");
			return;
		}
	}
	if (GW.fs) {
		DEBUG("go Windowed");
		set_window(GW.w_window);
		glfwDestroyWindow(GW.fs_window);
		GW.fs_window = 0;
	} else {
		DEBUG("go FS");
		set_window(GW.fs_window);
		//glfwHideWindow(gw.w_window);
	}
	glfwShowWindow(GW.window);
	
	GW.fs = !GW.fs;
}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	//~ int x = xpos*256.0/GW.w;
	//~ int y = ypos*144.0/GW.h;
	//~ x = (x<0)?0 : (x >255?255:x);
	//~ y = (y<0)?0 : (y >143?143:y);
	
	//~ GW.hoverX = xpos;
	//~ GW.hoverY = ypos;
	//~ input->touch[0].x = x;
	//~ input->touch[0].y = y;
}
void cursor_enter_callback(GLFWwindow* window, int entered)
{
	//~ if (!entered) {
		//~ input->hoverX = 255;
		//~ input->hoverY = 255;
	//~ }
}

static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	float monitor_aspect = (16.0/9.0);
	if (fabs((double)width /height - monitor_aspect) > 0.01) {
		//~ printf("BAD");
		if ((double) width/height > monitor_aspect)
			width = height*monitor_aspect;
		else
			height = width/monitor_aspect;
		if (width < 256) {
			width = 256;
			height = 144;
		}
		glfwSetWindowSize(window, width, height);
	} else {
		GW.w = width;
		GW.h = height;
		glViewport(0, 0, width, height);
	}
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	//~ if (action == GLFW_PRESS) {
		//~ if (!input->touch[0].buttons) { //first press
			//~ input->touch[0].x0 = input->touch[0].x;
			//~ input->touch[0].y0 = input->touch[0].y;
		//~ }
	//~ } 
	//~ if (action == GLFW_PRESS)
		//~ input->touch[0].buttons |= (1<<button);
	//~ else
		//~ input->touch[0].buttons &= ~(1<<button);
	
}
void character_callback(GLFWwindow* window, unsigned int codepoint)
{	
	if (codepoint > 0 && codepoint <='~' && GW.input_i < 256)
		GW.input_chars[GW.input_i++] = codepoint;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS && key == GLFW_KEY_ENTER && mods |GLFW_MOD_ALT ) {
		do_fullscreen();
		return;
	}
	if (action == GLFW_PRESS && key == GLFW_KEY_ESCAPE) {
		//input->status |= STATUS_CLOSE;
		return;
	}
	

	//~ if ((action == GLFW_PRESS || action == GLFW_REPEAT) && GW.input_i < 16) {
		//~ switch (key) {
			//~ case GLFW_KEY_ENTER: GW.input_chars[GW.input_i++] = '\n'; break;
			//~ case GLFW_KEY_TAB: GW.input_chars[GW.input_i++] = '\t'; break;
			//~ case GLFW_KEY_BACKSPACE: GW.input_chars[GW.input_i++] = '\b'; break;
			//~ default:break;
		//~ }
	//~ }
	//~ uint32_t *bitfield = 0;
	//~ int bit = 0;
	//~ if (key >= GLFW_KEY_A && key <= GLFW_KEY_Z) {
		//~ bit = key - GLFW_KEY_A;
		//~ bitfield = &input->alpha;
	//~ } else if (key >= GLFW_KEY_F1 && key <= GLFW_KEY_F16) {
		//~ bit = key - GLFW_KEY_F1;
		//~ bitfield = &input->function;
	//~ } else if (key >= GLFW_KEY_0   && key <= GLFW_KEY_9) {
		//~ bit = key - GLFW_KEY_0;
		//~ bitfield = &input->num;
	//~ } else if (key >= GLFW_KEY_KP_0   && key <= GLFW_KEY_KP_9) {
		//~ bit = key - GLFW_KEY_KP_0 + 16;
		//~ bitfield = &input->num;
	//~ }
	//~ if (bitfield) {
		//~ if (action == GLFW_PRESS)
			//~ *bitfield |= 1<<bit;
		//~ else if (action == GLFW_RELEASE)
			//~ *bitfield &= ~(1<<bit);
	//~ }	
}

Shader g_line_shader = {0};
GLuint g_line_vb=0;

void helper_gl_init(void)
{
	if (!g_line_shader.id) {
		if (!shader_init(&g_line_shader, V_STRAIGHT, F_SOLID, (char*[]){
			"aPos", "uScreen", "uColor", NULL}))
			ABORT(1, "Couldn't create g_line_shader shader");
		on_exit(shader_on_exit, &g_line_shader);
	}
	glDeleteBuffers(1, &g_line_vb);
	glGenBuffers(1, &g_line_vb);
	

}

void draw_color(float r, float g, float b, float a)
{
	GW.color[0] = r;
	GW.color[1] = g;
	GW.color[2] =b;
	GW.color[3] =a;
	
}

void drawl(GLenum mode, int npts, GLfloat *pts)
{
	//~ DEBUG("%d %d %f\n", g_line_shader.id, npts, pts[0]);
	glUseProgram(g_line_shader.id);
	glBindBuffer(GL_ARRAY_BUFFER, g_line_vb);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 2*npts, pts, GL_STATIC_DRAW);
	glVertexAttribPointer(g_line_shader.args[0], 2, GL_FLOAT, 0, 0, 0);
	glEnableVertexAttribArray(g_line_shader.args[0]);
	
	//~ GW.vmat = (GLfloat[3][3]){1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0};
	//~ GW.color = (GLfloat[4]){1.0, 1.0, 0.0, 1.0};
	
	glUniformMatrix3fv(g_line_shader.args[1], 1, 1, &GW.vmat[0][0]);// uScreen
	glUniform4fv(g_line_shader.args[2], 1, GW.color); //uColor
	glDrawArrays(mode, 0, npts);
}
void draw_line_strip(int npts, GLfloat *pts)
{
	drawl(GL_LINE_STRIP, npts, pts);
}
void draw_line_loop(int npts, GLfloat *pts)
{
	drawl(GL_LINE_LOOP, npts, pts);
}
void draw_lines(int npts, GLfloat *pts)
{
	drawl(GL_LINES, npts, pts);
}

void set_window(GLFWwindow *w)
{
	GW.window = w;
	glfwMakeContextCurrent(GW.window);

	//~ init_gl();
	//~ glfwSwapInterval(0);
	//~ glfwSetWindowAspectRatio(gw.window, 16, 9);
	//~ glfwSetWindowSizeLimits(gw.window, 256, 144, GLFW_DONT_CARE, GLFW_DONT_CARE);
	glfwSetFramebufferSizeCallback(GW.window, framebuffer_size_callback);
	//~ glfwSetKeyCallback(window, key_callback);
	//~ glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, 1);
	glfwSetKeyCallback(GW.window, key_callback);
	glfwSetCharCallback(GW.window, character_callback);
	glfwSetCursorPosCallback(GW.window, cursor_position_callback);
	glfwSetCursorEnterCallback(GW.window, cursor_enter_callback);
	glfwSetMouseButtonCallback(GW.window, mouse_button_callback);

	int width, height;
	glfwGetFramebufferSize(GW.window, &width, &height);
	framebuffer_size_callback(GW.window, width, height);
	gl_init();
	helper_gl_init();
	DEBUG("Initialization Complete");
	// set up a framebuffer for this new context
	//~ if (gw.fbobj)
		//~ ABORT(9, "fbobj should have been released");
	//~ glGenFramebuffers(1, &gw.fbobj);
	//~ glBindFramebuffer(GL_FRAMEBUFFER, gw.fbobj);
	//~ glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gw.fbtex, 0);
	//~ if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		//~ fprintf(stderr, "Couldn't get framebuffer (%d) ready! %d %d %d %d\n", gw.fbobj, glCheckFramebufferStatus(GL_FRAMEBUFFER),
		//~ GL_FRAMEBUFFER_COMPLETE, GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT,GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS);
	//~ glBindFramebuffer(GL_FRAMEBUFFER, 0); // put the default framebuffer back
}

static void error_callback(int error, const char* description)
{
	ABORT(100, "GLFW:%d: %s", error, description);
}

void win_on_exit(int status, void *arg)
{
	if (GW.fs_window) {
		DEBUG("Destroy FS window");
		glfwDestroyWindow(GW.fs_window);
	}
	if (GW.w_window) {
		DEBUG("Destroy Window");
		glfwDestroyWindow(GW.w_window);
	}
	DEBUG("GLFW Terminate");
	glfwTerminate();
	memset(&GW, 0, sizeof(Window));
}

int main(int argc, char *argv[])
{
	memset(&GW, 0, sizeof(Window));
	GW.zoom = 1.0;
	
	glfwSetErrorCallback(error_callback);
	if (!glfwInit())
		ABORT(1, "glfwinit");
	
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	
	GW.w_window = glfwCreateWindow(16*32, 9*32, gl_name, NULL, NULL);
	if (!GW.w_window) {
		glfwTerminate();
		ABORT(2, "glfwCreateWindow");
	}

	set_window(GW.w_window);
	DEBUG("GL_VERSION  : %s", glGetString(GL_VERSION) );
	DEBUG("GL_RENDERER : %s", glGetString(GL_RENDERER) );
	
	on_exit(win_on_exit, 0);
	
	glfwSetTime (0.0);
	while(!glfwWindowShouldClose(GW.window)) {
		glfwPollEvents();
		GW.vmat[0][0] = 2.0*GW.zoom / GW.w;
		GW.vmat[1][1] = 2.0*GW.zoom / GW.h;
		GW.vmat[2][2] = 1.0;
		GW.vmat[0][2] = GW.camx / GW.w;
		GW.vmat[1][2] = GW.camy / GW.h;
		
		gl_frame();
		++GW.frame;
		glfwSwapBuffers(GW.window);
	}
	ABORT(0, "Goodbye");
	
}

#endif
#endif


