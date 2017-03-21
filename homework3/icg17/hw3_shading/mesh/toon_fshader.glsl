#version 330

in vec3 normal_mv;
in vec3 light_dir;
in vec3 view_dir;

uniform vec3 La, Ld, Ls;
uniform vec3 ka, kd, ks;
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

    color += La*ka;
    vec3 n = normalize(normal_mv);
    vec3 l = normalize(light_dir);
    float lambert = dot(n,l);
    vec3 color_nl = texture(tex1D, max(dot(n,l), 0.0f)).rgb;
    color += Ld*kd*color_nl;
    vec3 v = normalize(view_dir);
    vec3 r = reflect(-l,n);
    float pow = pow(max(dot(r,v), 0.0f), alpha);
    vec3 color_rv = texture(tex1D, pow).rgb;
    color += Ls*ks*color_rv;
}
