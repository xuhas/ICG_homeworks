#version 330

in vec2 uv;
in float height;
in vec3 light_dir;
in vec4 vpoint_mv;
in float water_level;

uniform sampler2D tex_sand;
uniform sampler2D tex_grass;
uniform sampler2D tex_rock;
uniform sampler2D tex_snow;
uniform float time;

out vec3 color;

vec3 Ld = vec3(1,1,1);
vec3 kd = vec3(0.3,0.3,0.3);

void main() {
    vec3 col_low = vec3(0, 0.45, 0); //green
    vec3 col_high = vec3(0.3, 0.2, 0);//(102/256, 51/256, 0);
    float water_limit = 0.15; //water_level;
    float beach_limit = water_limit + 0.05;
    float snow_limit = 0.5;
    float rock_limit = 0.37;

    bool colours = false; //use colours

    if (colours == true){
        if(height <= water_limit) //water
            color = vec3(0.0,0.0,1.0); //blue
        else if((height > rock_limit) && (height < snow_limit)) //rocks
            color = col_high;
        else if(height > snow_limit) //snow
            color = vec3(1.0,1.0,1.0); //white
        else{
            float height_norm = height - water_limit; //now starts from 0
            float diff_height = rock_limit - water_limit;

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
    }
    else{
        if(height > snow_limit){ //snow
            if (height < snow_limit + 0.1){ //blend snow and rocks
                float height_coeff = (height - snow_limit) / 0.1; //mapped from 0 to 1
                color = texture(tex_snow, uv).rgb * height_coeff + texture(tex_rock, uv).rgb * (1.0 - height_coeff);
            }
            else
                color = texture(tex_snow, uv).rgb; //only snow
        }
        else if (height > rock_limit) //only rocks
            color =  texture(tex_rock, uv).rgb;
        else if (height > beach_limit){ //mix rocks and grass between beach_limit and snow_limit
            float height_coeff = (height - beach_limit) / (rock_limit - beach_limit); //mapped from 0 to 1
            color = texture(tex_rock, uv).rgb * height_coeff + texture(tex_grass, uv).rgb * (1.0 - height_coeff);
        }
        else if (height > water_limit){  //beach
            float height_coeff = (height - water_limit) / (beach_limit - water_limit); //mapped from 0 to 1
            color =  texture(tex_grass, uv).rgb * height_coeff + texture(tex_sand, uv).rgb * (1.0 - height_coeff);
        }
        else
            color =  texture(tex_sand, uv).rgb;
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
