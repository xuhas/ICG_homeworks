#version 330
//#include <GLFW/glfw3.h>
//#include<stdlib.h>

in vec2 position;

uniform mat4 model;
uniform mat4 view;
uniform mat4 MVP;
uniform sampler2D tex;
uniform float time;
uniform float SPEED; //param.h
uniform vec3 LIGHT_POS; //param.h

out float height;
out vec3 light_dir;
out vec4 vpoint_mv;
out vec2 uv;

void main() {

	uv = (position + vec2(1.0, 1.0)) * 0.5; //now uv goes from 0.0 to 1.0
        height = texture(tex, uv).x;

	uv = uv * 2 - time * SPEED; //TODO TRY TO REMOVE THE *2

	vec3 pos_3d = vec3(position.x, position.y, height);

	gl_Position = MVP * vec4(pos_3d, 1.0);


    //diffuse shading.
    ///compute the light direction light_dir
    mat4 MV = view * model;
    vpoint_mv = MV * vec4(pos_3d, 1.0);
	light_dir = normalize(LIGHT_POS.xyz - vpoint_mv.xyz);
}
