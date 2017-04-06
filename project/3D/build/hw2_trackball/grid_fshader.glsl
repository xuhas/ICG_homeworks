#version 330

in vec2 uv;

out vec3 color;

uniform sampler2D tex;

void main() {
     vec3 col= vec3(0.3,0.2,0.1);
    color = col;
}
