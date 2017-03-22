#version 330

in vec3 normal_mv;
in vec3 light_dir;
in vec3 view_dir;

out vec3 color;

uniform float alpha;
uniform sampler2D tex2D;

void main() {
    color = vec3(0.0, 0.0, 0.0);

//    const vec3 COLORS[6] = vec3[](
//        vec3(1.0,0.0,0.0),
//        vec3(0.0,1.0,0.0),
//        vec3(0.0,0.0,1.0),
//        vec3(1.0,1.0,0.0),
//        vec3(0.0,1.0,1.0),
//        vec3(1.0,0.0,1.0));
//    int index = int( mod(gl_PrimitiveID,6) );
//    color = COLORS[index];

    //>>>>>>>>>> TODO >>>>>>>>>>>
    // TODO 3.2: Artistic shading.
    // 1) compute the output color by doing a look-up in the texture using the
    //    texture sampler tex.
    //<<<<<<<<<< TODO <<<<<<<<<<<

    vec3 n = normalize(normal_mv);
    vec3 l = normalize(light_dir);
    vec3 v = normalize(view_dir);
    vec3 r = reflect(-l,n);
    vec2 tex = vec2(dot(n, l), pow(dot(r, v), alpha));
    color += texture(tex2D, tex).rgb;
}
