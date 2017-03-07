#version 330

in vec2 uv;

out vec3 color;

uniform sampler1D colormap;

void main() {
//    float value = sin(uv[0]);
    float PI = 3.14159265;
    float x = uv[0] * 10 * PI;
    float y = uv[1] * 10 * PI;

    float variationx = sin(x + y - 0.5*PI) + 1;
    float variationy = sin(x - y + 0.5*PI) + 1;
    float variation = (variationx + variationy) / 4;
    color = texture(colormap, variation).rgb;
}


