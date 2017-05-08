#pragma once

//grid
#define GRID_DIM 512
#define LIGHT_POS glm::vec3(0.0f, 0.0f, 2.0f)
#define SPEED 0.5f
#define WATER_HEIGHT 0.15f
#define BEACH_HEIGHT WATER_HEIGHT+0.05f
#define ROCK_HEIGHT 0.37f
#define SNOW_HEIGHT 0.5f
#define USE_COLOURS false

//main
    //keys Y/U to move along Y axe
    //keys X/C to move along X axe

//water
#define WATER_DIM 256

//trackball
#define ZOOM_SENSITIVITY 1
#define ROTATION_SENSITIVITY 1.5f


//I found that this values for the matrixes give kind of the "infinite terrain" view
//view matrix:
//mat4x4((1.000000, 0.000000, 0.000000, 0.000000), (0.000000, 1.000000, 0.000000, 0.000000), (0.000000, 0.000000, 1.000000, 0.000000), (0.000000, 0.000000, -1.160000, 1.000000))
//trackball matrix:
//mat4x4((-0.692370, -0.414809, 0.590387, 0.000000), (0.721505, -0.406323, 0.560653, 0.000000), (0.007324, 0.814147, 0.580613, 0.000000), (0.000000, 0.000000, 0.000000, 1.000000))
