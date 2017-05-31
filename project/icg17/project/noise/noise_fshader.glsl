#version 330
/* Most of the code in inspired from an article on github:
https://gist.github.com/patriciogonzalezvivo/670c22f3966e662d2f83

We have tried and tested many different noise patterns and these are the
ones that give a good result. At first, it may seem taht the result of the
noise is deterministic but since there is a time variable, it can vary every run.
We can switch from perlin noise and fractal Brownian noise by changind the
function in the main. We may implement changing by key later on*/

in vec2 position;

out vec3 color;

uniform mat4 MVP;
uniform float time;
uniform float SPEED; //param.h

//fractional brownian octaves
int NUM_OCTAVES=10; //before it was 100, but it is very heavy!
                    //it seems that doesn t change anything actually

//A rand function
float rand(vec2 n) {
    return fract(sin(dot(n, vec2(12.9898, 4.1414))) * 43758.5453);
}

//Some Perlin noise fuction
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

void main(){
	vec2 uv = (position + vec2(1.0, 1.0)) * 0.5; //now uv goes from 0.0 to 1.0

	//float height = fbm(position/5-time*SPEED)/3 -0.15;

	//the arg of fbm must be the same on grid_vshader (only the arg)
        float height = fbm(uv*10-time*SPEED)/4 -0.06;

    color = vec3(height, 0.0f, 0.0f);
}






