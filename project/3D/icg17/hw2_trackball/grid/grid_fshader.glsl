#version 330

in vec2 uv;
in float height;
out vec3 color;

uniform sampler2D tex;

void main() {
     vec3 col_white= vec3(1,1,1);
     vec3 col_black= vec3(0,0,0);

  if(height==0.05){color = vec3(0.0,0.0,1.0);}
  else if(height>0.5){color = vec3(1.0,1.0,1.0);}
  else{color = vec3(0.1, 0.8, 0.2);}

}
