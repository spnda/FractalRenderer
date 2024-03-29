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

vec3 backgroundGradient(float colour, ivec2 pixelCoords) {
	vec2 pt = pixelCoords;
	vec3 color1 = vec3(1.0, 0.55, 0.0) * colour;
	vec3 color2 = vec3(0.226, 0.0, 0.615) * colour;
	float mixValue = distance(pt, vec2(0,1));
	return vec3(mix(color1, color2, mixValue));
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

	// https://en.wikipedia.org/wiki/Julia_set for more examples to put instead of dvec2(-0.5, 0.156)
	float fractal = julia(coords.xy, vec2(-0.8, 0.156)) / MAX_ITERATIONS;
	vec3 colourOut = backgroundGradient(fractal, pixelCoords / iResolution.xy);

	imageStore(colorTex, pixelCoords, vec4(colourOut, 1.0));
}
