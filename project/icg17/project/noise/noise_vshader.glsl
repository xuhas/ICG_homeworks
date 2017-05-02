#version 330

in vec3 vpoint;
out vec2 uv;

void main() {
    gl_Position = mat4(mat3(1)) * vec4(vpoint, 1.0);
    uv = vec2(vpoint.x, vpoint.y);
}
