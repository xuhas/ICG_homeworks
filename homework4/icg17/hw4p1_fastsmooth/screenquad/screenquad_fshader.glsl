#version 330

in vec2 uv;

out vec3 color;

uniform sampler2D tex;
uniform float tex_width;
uniform float tex_height;
uniform float kernel[100];
uniform int size;

float rgb_2_luma(vec3 c) {
    return 0.3*c[0] + 0.59*c[1] + 0.11*c[2];
}

void main() {
#if 0
	//THIS PART IS FOR EDGE DETECTION, IT WON'T BE COMPILED, ONLY A REFERENCE
    // x=-1
    float t_00 = rgb_2_luma( textureOffset(tex, uv, ivec2(-1, -1)).rgb );
    float t_01 = rgb_2_luma( textureOffset(tex, uv, ivec2(-1,  0)).rgb );
    float t_02 = rgb_2_luma( textureOffset(tex, uv, ivec2(-1, +1)).rgb );
    // x=0
    float t_10 = rgb_2_luma( textureOffset(tex, uv, ivec2( 0, -1)).rgb );
    float t_12 = rgb_2_luma( textureOffset(tex, uv, ivec2( 0, +1)).rgb );
    // x=+1
    float t_20 = rgb_2_luma( textureOffset(tex, uv, ivec2(+1, -1)).rgb );
    float t_21 = rgb_2_luma( textureOffset(tex, uv, ivec2(+1,  0)).rgb );
    float t_22 = rgb_2_luma( textureOffset(tex, uv, ivec2(+1, +1)).rgb );
    // gradients
    float sx = -1*t_00 -2*t_01 -1*t_02    +1*t_20 +2*t_21 +1*t_22;
    float sy = -1*t_00 -2*t_10 -1*t_20    +1*t_02 +2*t_12 +1*t_22;
    float g = 1-sqrt(sx*sx + sy*sy);
    
    // composite
    // color = texture(tex,uv).rgb; ///< passthrough shading
    // color = abs( vec3(sx, sx, sx) ); ///< derivatives x
    // color = abs( vec3(sy, sy, sy) ); ///< derivatives y
    color = vec3(g, g, g);
#else
	vec3 color_tot = vec3(0,0,0);
	//ALONG X
	float weight_tot = 0;
	int start = (size - 1) / 2; //size is always odd
	for(int i=-start; i<=start; i++){
		vec3 neigh_color = texture(tex, uv+vec2(i/tex_width,0)).rgb;
		color_tot += kernel[i+start] * neigh_color;
		weight_tot += kernel[i+start];
	}
	color = color_tot;// / weight_tot;

	//ALONG Y
	//weight_tot = 0;
	for(int j=-start; j<=start; j++){
		vec3 neigh_color = texture(tex, uv+vec2(0,j/tex_height)).rgb;
		color_tot += kernel[j+start] * neigh_color;
		weight_tot += kernel[j+start];
	}
	color += color_tot / weight_tot;
#endif
}

