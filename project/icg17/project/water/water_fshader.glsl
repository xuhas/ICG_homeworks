#version 330

in vec2 uv;
in vec3 view_dir;
in vec3 light_dir;

out vec4 color;

uniform sampler2D water;

uniform float time;


void main() {

	//TODO add in param.h all the shading coeff
	float Ld = 0.2;
	float Ls = 0.1;

	//taking color from texture
	//TODO implement reflection, adding alpha to param.h not important since
	//     with the reflection we will not use it anymore
	color = vec4(texture(water,uv).rgb, 0.5); //ALPHA 0.5

	//Compute diffuse shading.
	vec3 dX = dFdx(view_dir.xyz);
	vec3 dY = dFdy(view_dir.xyz);
	vec3 n = normalize(cross(dX,dY));
	vec3 l = normalize(light_dir);
	float lambert = dot(n,l);

	//compute specular shading
	vec3 r_dir = reflect(-l, n);
	float rv = dot(r_dir, view_dir);
	rv = rv >= 0 ? rv : 0;

	if(lambert > 0.0) {//insuring that it's not hidden
		color += Ld*lambert;//diffuse term
		color += vec4(1) * (rv * Ls); //specular term
	}
	else
		color = vec4(0,0,0,0);

}
