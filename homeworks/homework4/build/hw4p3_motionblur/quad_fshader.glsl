#version 330 core
in vec2 uv;
in vec4 currentPosition;
in vec4 previousPosition;

uniform sampler2D tex;

layout (location = 0) out vec4 color;
layout (location = 1) out vec2 motionVector;

void main() {
    color = texture(tex, uv);
//#define STRIP_CODE
    // TODO: compute the screen space motion vector (in pixels!)
    // HINT: use straightforward finite differences and assume unit time-step
    // HINT: how do you compute pixels positions given homogeneous coordinate? (x,y,z,w)
    vec4 current_xyzw = currentPosition.xyzw;
    vec3 current_cube = vec3(current_xyzw.x / current_xyzw.w, current_xyzw.y / current_xyzw.w, current_xyzw.z / current_xyzw.w).xyx;
    vec2 current_screen = vec2(1024 * ((current_cube.x + 1) / 2), 768 * ((current_cube.y + 1) / 2)).xy;
    vec4 previous_xyzw = previousPosition.xyzw;
    vec3 previous_cube = vec3(previous_xyzw.x / previous_xyzw.w, previous_xyzw.y / previous_xyzw.w, previous_xyzw.z / previous_xyzw.w).xyx;
    vec2 previous_screen = vec2(1024 * ((previous_cube.x + 1) / 2), 768 * ((previous_cube.y + 1) / 2)).xy;
    motionVector = current_screen - previous_screen;
}
