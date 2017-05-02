#version 330

in vec2 uv;

out vec3 color;

uniform mat4 MVP;
uniform float time;

vec2 position;

//fractional brownian
int NUM_OCTAVES=6;

float rand(vec2 n) {
    return fract(sin(dot(n, vec2(12.9898, 4.1414))) * 43758.5453);
}

float noise(vec2 p){
    vec2 ip = floor(p);
    vec2 u = fract(p);
    u = u*u*(3.0-2.0*u);

    float res = mix(
                mix(rand(ip),rand(ip+vec2(1.0,0.0)),u.x),
                mix(rand(ip+vec2(0.0,1.0)),rand(ip+vec2(1.0,1.0)),u.x),u.y);
    return res*res;
}

float fbm(vec2 x) {
    float v = 0.0;
    float a = 0.5;
    vec2 shift = vec2(100);
    // Rotate to reduce axial bias
    mat2 rot = mat2(cos(0.5), sin(0.5), -sin(0.5), cos(0.50));
    for (int i = 0; i < NUM_OCTAVES; ++i) {
        v += a * noise(x);
        x = rot * x * 2.0 + shift;
        a *= 0.5;

    }
    return v;
}

void main() {
    float speedt = 0.5;
    float speedw = 1;
    float water_level=0.15;
    vec2 position = (uv + vec2(1.0, 1.0)) * 5;
    float height = fbm(position/5-time*speedt);
//    if(height<water_level){
//        height = water_level+fbm(uv*2+time*speedw/50)/100;//adjust inside fbm for movement of water
//    }
    color = vec3(height, 0.0f, 0.0f);
}
