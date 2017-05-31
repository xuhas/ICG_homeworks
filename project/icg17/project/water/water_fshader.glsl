#version 330

in vec2 uv;
in vec3 view_dir;
in vec3 light_dir;

out vec4 color;

uniform sampler2D water;
uniform sampler2D water_reflect;
uniform float time;


void main() {
	//TODO add in param.h all the shading coeff
        float Ld = 0.1;
        float Ls = 0.0;

	//TEXTURE
	{
		/// query window_width/height using the textureSize(..) function on tex_mirror
		vec2 size = textureSize(water_reflect,0);
		/// use gl_FragCoord to build a new [_u,_v] coordinate to query the framebuffer
		/// NOTE: make sure you normalize gl_FragCoord by window_width/height
		/// NOTE: you will have to flip the "v" coordinate as framebuffer is upside/down

		//normalizing
		float _u = gl_FragCoord.x/size[0];
		float _v = gl_FragCoord.y/size[1];

		///mix the texture(tex,uv).rgb with the value you fetch by texture(tex_mirror,vec2(_u,_v)).rgb
		vec3 color_from_texture = texture(water,uv).rgb; //from normal texture
		vec3 color_from_mirror = texture(water_reflect, vec2(_u,_v)).rgb; //from mirror

		//from mirror texture
		color  = vec4(mix(color_from_texture, color_from_mirror, vec3(.35)), 0.0f); //blend texture with mirror image
	}

	//SHADING
	{
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

		if(lambert > 0.0) { //insuring that it's not hidden
			color += Ld*lambert; //diffuse term
			color += (pow(rv,25) * Ls); //specular term
		}
		//else //WHY THIS IS NOT WORKING? I would like to make the water transparent when not seen (like under the mountains)
		    //color = vec4(0,0,0,0);
	}

	color = vec4(color.xyz, 0.7f); //ALPHA 0.6
}
