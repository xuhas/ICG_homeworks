#pragma once

//grid
#define GRID_DIM 2048
#define LIGHT_POS glm::vec3(0.0f, 0.0f, 3.0f)
#define SPEED 0.05f
#define WATER_HEIGHT 0.0f //IT HAS TO BE ZERO, OTHERWISE YOU FUCK UP THE REFLECTION
#define BEACH_HEIGHT WATER_HEIGHT+0.015f  // 0.015 ABSOLUTE VALUES
#define ROCK_HEIGHT BEACH_HEIGHT+0.1f    //  0.115
#define SNOW_HEIGHT ROCK_HEIGHT+0.02f    //  0.135
#define USE_COLOURS false
#define PACE 0.01f //lenght of pace when walking on terrain

//main
#define MOUSE_SENSITITY 0.003f
#define INERTIA 0.7; //not greater then 1

//water
#define WATER_DIM 256

//trackball
#define ZOOM_SENSITIVITY 1
#define ROTATION_SENSITIVITY 1.5f

//bezier
#define BEZIER_LENGTH 28000
#define BEZIER_DIVIDER 30000
