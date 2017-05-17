#version 330

in vec2 position;

out vec2 uv;
out vec3 view_dir;
out vec3 light_dir;

uniform mat4 MVP;
uniform mat4 MV;
uniform float WATER_HEIGHT; //param.h
uniform float SPEED; //param.h
uniform vec3 LIGHT_POS; //param.h
uniform float time;

void main() {
	uv = (position + vec2(1.0, 1.0)) - time * SPEED * 0.5; //0.5 is to have different speed between the water and the terrain
	                                                       //(simulate waves)

	//vec3 pos_3d = vec3(position.x, 0 + water_ampl * sin(time), position.y);
	vec3 pos_3d = vec3(position.x, position.y, WATER_HEIGHT);

	gl_Position = MVP * vec4(pos_3d, 1);

	///compute the light direction light_dir
    vec4 vpoint_mv = MV * vec4(pos_3d, 1.0);
	vec4 light_mv = MV * vec4(LIGHT_POS, 1.0);

	view_dir = normalize(-vpoint_mv.xyz);
	light_dir = normalize(light_mv.xyz - vpoint_mv.xyz);
}
