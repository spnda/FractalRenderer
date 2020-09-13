/*
 * spnda, Copyright (c) 2020
 * mandelbrot gradient fragment shader
 */
#version 460 core

uniform float iTime;
uniform float zoom;

uniform vec2 position;
uniform vec2 iResolution;

layout(location = 0) out vec4 colour;

int MAX_ITERATIONS = 100;

double mandelbrot(dvec2 z, dvec2 c) {
	for (int i = 0; i < MAX_ITERATIONS; i++) {
		dvec2 r = z * z;
		if (r.x + r.y > 4.0) return i;
		z = dvec2(r.x - r.y + c.x, 2.0 * z.x * z.y + c.y);
	}
	return MAX_ITERATIONS;
}

vec3 backgroundGradient(double colour) {
	vec2 pt = gl_FragCoord.xy/vec2(iResolution);
	dvec3 color1 = dvec3(1.0, 0.55, 0.0) * colour;
	dvec3 color2 = dvec3(0.226, 0.0, 0.615) * colour;
	double mixValue = distance(pt, vec2(0,1));
	return vec3(mix(color1, color2, mixValue));
}

double translateCoordinates(double val, int maxV, int minV) {
	return ((val - (minV)) / (maxV - (minV))) * 4.0 - 2.0;
}

void main(void) {
	double x = translateCoordinates(int(gl_FragCoord.x), int(iResolution.x), 0);
	double y = translateCoordinates(int(gl_FragCoord.y), int(iResolution.y), 0);
	x = x * zoom + position.x;
	y = y * zoom + position.y;
	double fractal = mandelbrot(dvec2(x, y), dvec2(x, y)) / MAX_ITERATIONS;
	vec3 colourOut = backgroundGradient(fractal);
	colour = vec4(colourOut, 1.0f);
}
