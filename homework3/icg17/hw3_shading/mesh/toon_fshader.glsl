#version 330
uniform vec3 La, Ld, Ls;
uniform vec3 ka, kd, ks;

in vec3 normal_mv;
in vec3 light_dir;
in vec3 view_dir;

uniform float alpha;
uniform mat4 projection;
uniform sampler1D tex1D;

out vec3 color;

void main() {
    color = vec3(0.0,0.0,0.0);

    ///>>>>>>>>>> TODO >>>>>>>>>>>
    /// TODO 2.2: Toon shading.
    /// 1) compute ambient term.
    /// 2) compute diffuse term using the texture sampler tex.
    /// 3) compute specular term using the texture sampler tex.
    ///<<<<<<<<<< TODO <<<<<<<<<<<

//    vec3 n = normalize(normal_mv);
//    vec3 l = normalize(light_dir);
//    vec3 color_nl = texture(tex1D, max(dot(n,l), 0.0f)).rgb;
//    color += color_nl;
//    vec3 v = normalize(view_dir);
//    vec3 r = reflect(-l,n);
//    vec3 color_rv = texture(tex1D, max(dot(r,v), 0.0f)).rgb;
//    color += color_rv;

    color += ka*La;//ambient term
    vec3 n = normalize(normal_mv);
    vec3 l = normalize(light_dir);
    vec3 lambert = texture(tex1D,dot(n,l)).rgb;
   // if(lambert > 0.0) {
        color += kd*lambert*Ld;//diffuse term
        vec3 v = normalize(view_dir);
        vec3 r = reflect(-l,n);
        vec3 color_rv = texture(tex1D, max(dot(r,v), 0.0f)).rgb;//specular term
        color += ks*color_rv*Ls;
        color += Ls*ks*pow(max(dot(r,v), 0.0), alpha);//for the specular detail
   // }
}
