#version 330

uniform vec3 La, Ld, Ls;
uniform vec3 ka, kd, ks;
uniform float alpha;
uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform vec3 light_pos;

in vec3 light_dir;
in vec3 view_dir;
in vec3 normal_mv;


out vec3 color;


uniform sampler1D tex1D;

void main() {
    color = vec3(0.0,0.0,0.0);

    ///>>>>>>>>>> TODO >>>>>>>>>>>
    /// TODO 2.2: Toon shading.
    /// 1) compute ambient term.
    /// 2) compute diffuse term using the texture sampler tex.
    /// 3) compute specular term using the texture sampler tex.
    ///<<<<<<<<<< TODO <<<<<<<<<<<

    color += La*ka;
    vec3 n = normalize(normal_mv);
    vec3 l = normalize(light_dir);
    float lambert = dot(n,l);
    if(lambert > 0.0) {
        color += Ld*kd*lambert;
        vec3 v = normalize(view_dir);
        vec3 r = reflect(-l,n);
        color += Ls*ks*pow(max(dot(r,v), 0.0), alpha);
    }
}
