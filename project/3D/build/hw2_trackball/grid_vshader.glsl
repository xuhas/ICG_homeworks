#version 330
//#include <GLFW/glfw3.h>
//#include<stdlib.h>
in vec2 position;

out vec2 uv;

uniform float randheight;

uniform mat4 MVP;
uniform float time;

void main() {
float speed = 100;

    uv = (position + vec2(1.0, 1.0)) * 0.5;

    // convert the 2D position into 3D positions that all lay in a horizontal
    // plane.
    // TODO 6: animate the height of the grid points as a sine function of the
    // 'time' and the position ('uv') within the grid.
    //float height = sin(radians((10*(uv[0]+uv[1]))+time)*speed)/10;

    float height = randheight*(radians(((uv[0]+uv[1])))*speed)/10; ;
    vec3 pos_3d = vec3(position.x, height, -position.y);

    gl_Position = MVP * vec4(pos_3d, 1.0);
}
