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
in vec4 vpoint_mv;
out vec3 color;


void main() {

    ///>>>>>>>>>> TODO >>>>>>>>>>>
    /// TODO 4.2: Flat shading.
    /// 1) compute triangle normal using dFdx and dFdy
    /// 1) compute ambient term.
    /// 2) compute diffuse term.
    /// 3) compute specular term.
    ///<<<<<<<<<< TODO <<<<<<<<<<<
            vec3 dX = dFdx(vpoint_mv.xyz);
            vec3 dY = dFdy(vpoint_mv.xyz);
            vec3 n = normalize(cross(dX,dY));

            color += La*ka;//ambient term
            vec3 l = normalize(light_dir);
            float lambert = dot(n,l);
            if(lambert > 0.0) {
                    color += Ld*kd*lambert;//diffuse term
                    vec3 v = normalize(view_dir);
                    vec3 r = reflect(-l,n);
                    color += Ls*ks*pow(max(dot(r,v), 0.0), alpha);//specular term
            }
}
