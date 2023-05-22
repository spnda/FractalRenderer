#version 460 core

uniform float iTime;
uniform float zoom;

uniform vec2 position;
uniform ivec2 iResolution;

layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;

layout(binding = 0, rgba8) writeonly uniform image2D colorTex;

int MAX_ITERATIONS = 100;

float julia(vec2 z, vec2 c) {
	for (int i = 0; i < MAX_ITERATIONS; i++) {
		vec2 r = z * z;
		if (r.x + r.y > 4.0) return i;
		z = vec2(r.x - r.y + c.x, 2.0 * z.x * z.y + c.y);
	}
	return MAX_ITERATIONS;
}

vec3 rgb(float ratio) {
	float normalized = ratio * 256 * 6;
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

float translateCoordinates(float val, int maxV, int minV) {
	return ((val - (minV)) / (maxV - (minV))) * 2.0 - 1.0;
}

void main() {
	ivec2 pixelCoords = ivec2(gl_GlobalInvocationID.xy);

	int m = min(iResolution.x, iResolution.y);
	ivec2 mdiff = (iResolution - m) / 2;
	vec2 coords = vec2(translateCoordinates(pixelCoords.x, m + mdiff.x, mdiff.x),
					   translateCoordinates(pixelCoords.y, m + mdiff.y, mdiff.y));

	float fractal = julia(coords.xy, coords.xy) / MAX_ITERATIONS;
	vec3 colourOut = rgb(fractal);

	imageStore(colorTex, pixelCoords, vec4(colourOut, 1.0));
}
