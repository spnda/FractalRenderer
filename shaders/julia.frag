/*
 * spnda, Copyright (c) 2020
 * julia fragment shader
 */
#version 460 core

uniform int height; // total height in pixels
uniform int width; // total width in pixels

uniform float time;
uniform float animationCounter; // animation value between -2 and 2
uniform float zoom;
uniform float positionX;
uniform float positionY;

layout(location = 0) out vec4 colour;

int MAX_ITERATIONS = 100; // quality (higher = slower)

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
		case 0: red = 255;      grn = x;        blu = 0;       break;//red
		case 1: red = 255 - x;  grn = 255;      blu = 0;       break;//yellow
		case 2: red = 0;        grn = 255;      blu = x;       break;//green
		case 3: red = 0;        grn = 255 - x;  blu = 255;     break;//cyan
		case 4: red = x;        grn = 0;        blu = 255;     break;//blue
		case 5: red = 255;      grn = 0;        blu = 255 - x; break;//magenta
    }
	return vec3(red, grn, blu) / 255.0;
}

vec3 backgroundGradient(double colour) {
	vec2 pt = gl_FragCoord.xy/vec2(width, height).xy;
	dvec3 color1 = dvec3(1.0, 0.55, 0.0) * colour;
	dvec3 color2 = dvec3(0.226, 0.0, 0.615) * colour;
	double mixValue = distance(pt, vec2(0,1));
	return vec3(mix(color1, color2, mixValue));
}

void main(void) {
	double x = (gl_FragCoord.x - 0.5 - (0)) / (width - (-2)) * 4.0 - 2.0;
    double y = (gl_FragCoord.y - 0.5 - (0)) / (height - (-2)) * 4.0 - 2.0;
    x = x * zoom + positionX;
    y = y * zoom + positionY;
                                            //-0.8, 0.156 is a example julia set.
                                            //https://en.wikipedia.org/wiki/Julia_set
	double fractal = julia(dvec2(x, y), dvec2(-0.8, 0.156)) / MAX_ITERATIONS;
	vec3 colourOut = backgroundGradient(fractal); 
                // rgb or backgroundGradient can be used here
                // to achieve either a gradient based on value
                // or a nice-looking gradient
	colour = vec4(colourOut, 1.0f);
}
