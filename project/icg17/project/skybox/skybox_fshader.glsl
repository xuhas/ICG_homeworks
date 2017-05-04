#version 330

in vec3 uv;
out vec4 color;
uniform samplerCube cubemap;

void main() {
        color = texture(cubemap, uv);

}
