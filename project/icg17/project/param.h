#pragma once

//grid
#define GRID_DIM 512
#define LIGHT_POS glm::vec3(0.0f, 0.0f, 3.0f)
#define SPEED 0.1f
#define WATER_HEIGHT 0.0f //before was -0.1f //IT HAS TO BE ZERO, OTHERWISE YOU FUCK UP THE REFLECTION
#define BEACH_HEIGHT WATER_HEIGHT+0.05f  // 0.05 ABSOLUTE VALUES
#define ROCK_HEIGHT BEACH_HEIGHT+0.17f   //  0.22
#define SNOW_HEIGHT ROCK_HEIGHT+0.13f    //  0.35
#define USE_COLOURS false

//main
    //keys WASD to move ---- TODO implement better
    //SPACE BAR to stop time ----- TODO implement better

//water
#define WATER_DIM 256

//trackball
#define ZOOM_SENSITIVITY 1
#define ROTATION_SENSITIVITY 1.5f
