#version 330

in vec2 uv; //needed??
in float height;
in vec3 light_dir;
in vec4 vpoint_mv;
in float water_level;
vec3 Ld = vec3(1,1,1);
vec3 kd = vec3(0.3,0.3,0.3);

out vec3 color;

void main() {
	vec3 col_low = vec3(0.3, 0.6, 0.3);
	vec3 col_high = vec3(0.8, 0.6, 0.3);
        float water_limit = water_level;
	float snow_limit = 0.5;

	if(height <= water_limit) //water
		color = vec3(0.0,0.0,1.0);
	else if(height > snow_limit) //snow
		color = vec3(1.0,1.0,1.0);
	else{
		float height_norm = height - water_limit; //now starts from 0
		float diff_height = snow_limit - water_limit;

		//interpolate color x
		float diff_color_x = col_high.x - col_low.x;
		float ratio_x = diff_color_x / diff_height;
		float color_x = col_low.x + height_norm * ratio_x;

		//interpolate color y
		float diff_color_y = col_high.y - col_low.y;
		float ratio_y = diff_color_y / diff_height;
		float color_y = col_low.y + height_norm * ratio_y;

		//interpolate color z
		float diff_color_z = col_high.z - col_low.z;
		float ratio_z = diff_color_z / diff_height;
		float color_z = col_low.z + height_norm * ratio_z;

		color = vec3(color_x, color_y, color_z);
	}

	//Compute diffuse shading.
	vec3 dX = dFdx(vpoint_mv.xyz);
	vec3 dY = dFdy(vpoint_mv.xyz);
	vec3 n = normalize(cross(dX,dY));
	vec3 l = normalize(light_dir);
	float lambert = dot(n,l);
	if(lambert > 0.0) {//insuring that it's not hidden
		color += Ld*kd*lambert;//diffuse term
	}
}
