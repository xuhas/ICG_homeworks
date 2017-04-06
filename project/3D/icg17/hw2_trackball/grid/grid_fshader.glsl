#version 330

in vec2 uv;
in float height;
out vec3 color;

uniform sampler2D tex;

void main() {
     vec3 col_white= vec3(1,1,1);
     vec3 col_black= vec3(0,0,0);
     //color = col_white * height *  1000000;
  if(height==0){color = vec3(0.0,0.0,0.0);}
  else{color = vec3(0.3, 0.2, 0.1);}
   // color = vec3(0.3, 0.2, 0.1);
}
