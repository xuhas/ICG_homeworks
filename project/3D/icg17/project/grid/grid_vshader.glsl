#version 330
//#include <GLFW/glfw3.h>
//#include<stdlib.h>

in vec2 position;

uniform mat4 model;
uniform mat4 view;
uniform mat4 MVP;
uniform sampler2D tex;
uniform float time;

out float height;
out vec3 light_dir;
out vec4 vpoint_mv;
out vec2 uv; //needed??
out float water_level;

vec3 light_pos= vec3(0,0,2);



void main() {
    float speedt = 0.5;
    float speedw = 1;
    water_level=0.15;
    uv = (position + vec2(1.0, 1.0)) * 0.5;

    height = texture(tex, uv).x;
    uv = uv * 2 - time * speedt;
    vec3 pos_3d = vec3(0.0,0.0,0.0);


    pos_3d = vec3(position.x, -position.y, height);

    gl_Position = MVP * vec4(pos_3d, 1.0);


    //diffuse shading.
    ///compute the light direction light_dir
    mat4 MV = view * model;
    vpoint_mv = MV * vec4(pos_3d, 1.0);
    light_dir = light_pos - vpoint_mv.xyz;


}
