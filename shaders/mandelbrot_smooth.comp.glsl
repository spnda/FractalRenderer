// https://www.shadertoy.com/view/4df3Rn
// Created by inigo quilez - iq/2013
// License Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.

// See here for more information on smooth iteration count:
//
// http://iquilezles.org/www/articles/mset_smooth/mset_smooth.htm

#version 460 core

uniform float iTime;
uniform float zoom;

uniform vec2 position;
uniform ivec2 iResolution;

layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;

layout(binding = 0, rgba8) writeonly uniform image2D colorTex;

#define AA 2 // increase this if you have a very fast GPU

float mandelbrot(in vec2 c) {
    #if 1 
    {
        float c2 = dot(c, c);
        // skip computation inside M1 - http://iquilezles.org/www/articles/mset_1bulb/mset1bulb.htm
        if (256.0 * c2 * c2 - 96.0 * c2 + 32.0 * c.x - 3.0 < 0.0 ) return 0.0;
        // skip computation inside M2 - http://iquilezles.org/www/articles/mset_2bulb/mset2bulb.htm
        if (16.0 * (c2 + 2.0 * c.x + 1.0) - 1.0 < 0.0 ) return 0.0;
    }
    #endif

    const float B = 256.0;
    float l = 0.0;
    vec2 z  = vec2(0.0);
    for(int i = 0; i < 512; i++) {
        z = vec2(z.x * z.x - z.y * z.y, 2.0 * z.x * z.y) + c;
        if (dot(z, z) > (B * B)) break;
        l += 1.0;
    }

    if (l > 511.0) return 0.0;
    
    float sl = l - log2(log2(dot(z, z))) + 4.0;

    return sl;
}

void main(void) {
    vec2 fragCoord = gl_GlobalInvocationID.xy;
    
    vec3 col = vec3(0.0);
    
    for(int m=0; m<AA; m++) {
        for(int n=0; n<AA; n++) {
            vec2 p = (-iResolution.xy + 2.0 * (fragCoord.xy + vec2(float(m), float(n)) / float(AA))) / iResolution.y;
            float w = float(AA * m + n);
            float time = iTime + 0.5 * (1.0 / 24.0) * w / float(AA * AA);
            float zoo = 0.62 + 0.38 * cos(.07 * time);
            float coa = cos(0.15 * (1.0 - zoo) * time);
            float sia = sin(0.15 * (1.0 - zoo) * time);
            zoo = pow(zoo, 8.0);

            vec2 xy = vec2(p.x * coa - p.y * sia, p.x * sia + p.y * coa);
            vec2 c = vec2(-.745, .186) + xy * zoo;

            float l = mandelbrot(c);

            col += 0.5 + 0.5 * cos(3.0 + l * 0.15 + vec3(0.0, 0.6, 1.0));
        }
    }

    col /= float(AA * AA);

	imageStore(colorTex, ivec2(fragCoord), vec4(col, 1.0));
}
