/*
 * spnda, Copyright (c) 2020
 * mandelbrot fragment shader
 */
#version 460 core

uniform float iTime;
uniform float zoom;

uniform vec2 position;
uniform vec2 iResolution;

layout(location = 0) out vec4 colour;

int MAX_ITERATIONS = 100;

double julia(dvec2 z, dvec2 c) {
	for (int i = 0; i < MAX_ITERATIONS; i++) {
		dvec2 r = z * z;
		if (r.x + r.y > 4.0) return i;
		z = dvec2(r.x - r.y + c.x, 2.0 * z.x * z.y + c.y);
	}
	return MAX_ITERATIONS;
}

vec3 rgb(double ratio) {
	double normalized = ratio * 256 * 6;
	int x = int(mod(normalized, 256.0));
	int red = 0, grn = 0, blu = 0;
	switch (int(normalized / 256)) {
		case 0: red = 255;		grn = x;		blu = 0;		break;//red
		case 1: red = 255 - x;	grn = 255;		blu = 0;		break;//yellow
		case 2: red = 0;		grn = 255;		blu = x;		break;//green
		case 3: red = 0;		grn = 255 - x;	blu = 255;		break;//cyan
		case 4: red = x;		grn = 0;		blu = 255;		break;//blue
		case 5: red = 255;		grn = 0;		blu = 255 - x;	break;//magenta
	}
	return vec3(red, grn, blu) / 255.0;
}

double translateCoordinates(double val, int maxV, int minV) {
	return ((val - (minV)) / (maxV - (minV))) * 4.0 - 2.0;
}

void main(void) {
	double x = translateCoordinates(int(gl_FragCoord.x), int(iResolution.x), 0);
	double y = translateCoordinates(int(gl_FragCoord.y), int(iResolution.y), 0);
	x = x * zoom + position.x;
	y = y * zoom + position.y;
	double fractal = julia(dvec2(x, y), dvec2(x, y)) / MAX_ITERATIONS;
	vec3 colourOut = rgb(fractal);
	colour = vec4(colourOut, 1.0f);
}
