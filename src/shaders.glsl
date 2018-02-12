////V_TRANSCALE
#version 100
precision mediump float;
attribute vec2 aPos;
uniform mat3 uScreen;
uniform vec2 uTranslate;
uniform vec2 uScale;
uniform float uAngle;

void main()
{
	float c = cos(uAngle);
	float s = sin(uAngle);
	vec2 pos2 = mat2(c,s,-s,c)*(aPos*uScale)+uTranslate;
	vec3 pos3 = uScreen*vec3(pos2, 1.0);
	gl_Position = vec4(pos3.xy, 0.0, 1.0);
}

////V_STRAIGHT
#version 100
precision mediump float;
attribute vec2 aPos;
uniform mat3 uScreen;

void main()
{
	vec3 pos3 = uScreen*vec3(aPos, 1.0);
	gl_Position = vec4(aPos, 0.0, 1.0);
}


////F_SOLID
#version 100
precision mediump float;
uniform vec4 uColor;

void main()
{
	gl_FragColor = uColor;
}

////
