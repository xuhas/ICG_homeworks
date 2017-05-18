#version 330

in vec3 vpoint;
out vec2 position;

void main() {
	gl_Position = vec4(vpoint, 1.0);
	position = vec2(vpoint.x, vpoint.y);
}
