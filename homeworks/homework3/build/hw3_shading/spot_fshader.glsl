#version 330
uniform vec3 La, Ld, Ls;
uniform vec3 ka, kd, ks;
uniform float alpha;
uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform vec3 light_pos;
uniform vec3 spot_dir;

in vec3 light_dir;
in vec3 view_dir;
in vec3 normal_mv;


out vec3 color;


const float spot_cos_cutoff = 0.985; // cos 10 degrees
const float spot_exp = 150;

void main() {    

    //normaizing the vectors
    vec3 n = normalize(normal_mv);
    vec3 l = normalize(light_dir);
    vec3 s=normalize(spot_dir);

    //calculating SpotEffect
    float SpotEffect = pow(dot(l,s),spot_exp);

    //Verifing if the segment is within the cone of light
    if(dot(s,l)/(length(s)*length(l))>= spot_cos_cutoff){
        float lambert = dot(n,l);
        if(lambert > 0.0) {
            vec3 v = normalize(view_dir);
            vec3 r = reflect(-l,n);
            //applying the spot effect to Phong shading
            color += (Ld*kd*lambert+Ls*ks*pow(max(dot(r,v), 0.0), alpha))*SpotEffect;
         }
      }
    //if not, it is in the dark
    else{
        color = vec3(0.0,0.0,0.0);
       }
}
