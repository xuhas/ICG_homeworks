#version 330

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

    vec3 n = normalize(normal_mv);
    vec3 l = normalize(light_dir);
    vec3 color_nl = texture(tex1D, max(dot(n,l), 0.0f)).rgb;
    color += color_nl;
    vec3 v = normalize(view_dir);
    vec3 r = reflect(-l,n);
    vec3 color_rv = texture(tex1D, max(dot(r,v), 0.0f)).rgb;
    color += color_rv;
}
