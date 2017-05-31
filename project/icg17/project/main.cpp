/* This new main aims to reorginize things
 * in a good fashion by taking in account
 * all the camera modes and eventually the
 * different noise functions.*/

// glew must be before glfw
#include <GL/glew.h>
#include <GLFW/glfw3.h>
// contains helper functions such as shader compiler
#include "icg_helper.h"
#include <glm/gtc/matrix_transform.hpp>
//links to the different files
#include "grid/grid.h"
#include "noise/noise.h"
#include "skybox/skybox.h"
#include "framebuffer.h"
#include "trackball.h"
#include "water/water.h"
#include "param.h"
#include "glm/ext.hpp"
#include "bezier.h"

//defines for clearer use of functions in main.cpp
#define WATER_REFLECTION 1
#define NOT_DRAW_SAND false
#define LINEAR_INTERP true

Grid grid;
Noise noise;
Skybox skybox;
FrameBuffer noise_framebuffer;
FrameBuffer water_refl;
Water water;

//bezier variables
Bezier path;
Bezier direction;
bool b_path_start = true;
double bezier_time = 0;
float bezier_speed = 1;

//initial window dimensions
int window_width = 800;
int window_height = 600;
bool pause = false;
float stop_time = 0;
float altitude;//for FPS

//for some geomatrical functions
using namespace glm;

//matrixes for the view
mat4 projection_matrix;
mat4 view_matrix;
mat4 model_matrix;
vec3 cam_pos;
vec2 cam_speed; //on the x the FW-BW speed, on the y the LX-RX speed
vec3 view_pos;
vec3 view_dir;

// camera modes
enum CameraMode { DEFAULT, FIRST_PERSON_SHOOTER, FLY_THROUGH, BEZIER, INFINITE_TERRAIN};
CameraMode cam_mode = FLY_THROUGH;//creation of a camera mode, starts with default

//calculation of the fps during the animation
//prints every second the FPS
void calculate_fps(float time_now){
    static float n_frames = 0;
    static float time_last_print = 0;
    n_frames++;

    if (time_now - time_last_print >= 1.0) {
        cout << n_frames << " FPS" << endl;
        n_frames = 0;
        time_last_print = time_now;
    }
}
//Matrix of the orthographic projection
mat4 OrthographicProjection(float left, float right, float bottom,
                            float top, float near, float far) {
    assert(right > left);
    assert(far > near);
    assert(top > bottom);
    mat4 projection = mat4(1.0f);
    projection[0][0] = 2.0f / (right - left);
    projection[1][1] = 2.0f / (top - bottom);
    projection[2][2] = -2.0f / (far - near);
    projection[3][3] = 1.0f;
    projection[3][0] = -(right + left) / (right - left);
    projection[3][1] = -(top + bottom) / (top - bottom);
    projection[3][2] = -(far + near) / (far - near);
    return projection;
}

//Perspective projection matrix
mat4 PerspectiveProjection(float fovy, float aspect, float near, float far) {
    // Create a perspective projection matrix given the field of view,
    // aspect ratio, and near and far plane distances.
    assert(far > near);
    float top = near * tan(radians(fovy));
    float bottom = - top;
    float right = top * aspect;
    float left = -right;
    vec4 C0(0.0f);
    vec4 C1(0.0f);
    vec4 C2(0.0f);
    vec4 C3(0.0f);
    C0[0] = 2 * near / (right - left);
    C1[1] = 2 * near / (top - bottom);
    C2[0] = (right + left) / (right - left);
    C2[1] = (top + bottom) / (top - bottom);
    C2[2] = -(far + near) / (far - near);
    C2[3] = -1;
    C3[2] = -2 * far * near / (far - near);
    mat4 projection(C0, C1, C2, C3);
    return projection;
}

//Look at matrix defines the direction of the view
mat4 LookAt(vec3 eye, vec3 center, vec3 up) {
    vec3 z_cam = normalize(eye - center);
    vec3 x_cam = normalize(cross(up, z_cam));
    vec3 y_cam = cross(z_cam, x_cam);

    mat3 R(x_cam, y_cam, z_cam);
    R = transpose(R);

    mat4 look_at(vec4(R[0], 0.0f),
            vec4(R[1], 0.0f),
            vec4(R[2], 0.0f),
            vec4(-R * (eye), 1.0f));
    return look_at;
}

//initialisation of the program
void Init() {
    // sets background color

    glClearColor(0.9, 0.9, 1.0 /*gray*/, 0.5 /*solid*/);

    skybox.Init();
    noise.Init();
    GLuint framebuffer_noise_id = noise_framebuffer.Init(window_width, window_height,LINEAR_INTERP);
    GLuint framebuffer_water_refl_id = water_refl.Init(window_width,window_height,LINEAR_INTERP, WATER_REFLECTION);
    grid.Init(framebuffer_noise_id);
    water.Init(framebuffer_water_refl_id);

    // enable depth test.
    glEnable(GL_DEPTH_TEST);

    cam_pos = vec3(0.0f, 0.0f, 0.3f);
    cam_speed = vec2(0.0f, 0.0f);
    view_pos = vec3(0.0f, 0.0f, 0.0f);
    model_matrix = IDENTITY_MATRIX;
}

// transforms glfw screen coordinates into normalized OpenGL coordinates in [-1,1], the center of the screen is (0,0).
vec2 TransformScreenCoords(GLFWwindow* window, int x, int y) {
    // the framebuffer and the window doesn't necessarily have the same size
    // i.e. hidpi screens. so we need to get the correct one
    int width;
    int height;
    glfwGetWindowSize(window, &width, &height);
    return vec2(2.0f * (float)x / width - 1.0f, //x=0 becomes x=-1
                1.0f - 2.0f * (float)y / height); //y=0 becomes y=1
}

//returns the click of the mouse in screen coordinates
void MouseButton(GLFWwindow* window, int button, int action, int mod) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {

    }
}

//gives the position of the mouse in screen coordinates
void MousePos(GLFWwindow* window, double x, double y) {

}

// gets called for every frame.
void Display(GLFWwindow* window) {
    //calculate the FPS
    float time = (float)glfwGetTime();
    calculate_fps(time);
    if (pause)
        time = stop_time;

    if(cam_mode == FLY_THROUGH||FIRST_PERSON_SHOOTER){
        double x_i, y_i;
        glfwGetCursorPos(window, &x_i, &y_i);
        //compute horizontal and vertical angles
        float horizontalAngle = MOUSE_SENSITITY * float(-x_i);
        float verticalAngle = MOUSE_SENSITITY * float(-y_i);
        //a vector from spherical coords to cartesian
        view_dir = vec3(cos(horizontalAngle),
                        sin(horizontalAngle),
                        verticalAngle);
        cam_pos += glm::normalize(view_dir) * cam_speed.x;
        cam_pos += glm::cross(view_dir,vec3(0.0,0.0,1.0)) * cam_speed.y;
        view_pos = cam_pos + view_dir;
        view_matrix = lookAt(cam_pos, view_pos, vec3(0.0,0.0,1.0));
        cam_speed = cam_speed * INERTIA;

    }

    if(cam_mode == BEZIER) {
        vec3 position;
        vec3 cam_direction;
        if(b_path_start) {
            path.clean();
            direction.clean();

//            path.addPoint(vec3(0.0,0.0,0.5));
//            path.addPoint(vec3(0.5,0.5,1.0));
//            path.addPoint(vec3(-0.7,0,0.6));
//            path.addPoint(vec3(0.0, 0.6,1.0));
//            path.addPoint(vec3(0.0, 0.0,0.7));
//            path.addPoint(vec3(0.0, 0.0,1));

//            direction.addPoint(vec3(0.3,0.3,0));
//            direction.addPoint(vec3(0.4,0.4,0.4));
//            direction.addPoint(vec3(1,1,0.4));
//            direction.addPoint(vec3(0,0,1));

            path.addPoint(vec3(0.0,0.0,0.1));
            path.addPoint(vec3(0.3,0.4,0.2));
            path.addPoint(vec3(-0.7,0.3,0.3));
            path.addPoint(vec3(0.0,0.0,0.4));
            direction.addPoint(vec3(0.3,0.3,0));
            direction.addPoint(vec3(0.4,0.7,0.4));
            direction.addPoint(vec3(1,1,0.4));
            direction.addPoint(vec3(0,0,1));

            b_path_start = false;
        }
        bezier_time = (int) (bezier_time + 50 * bezier_speed) % BEZIER_LENGTH;
        double path_time = bezier_time/BEZIER_DIVIDER;
        double dir_time = bezier_time/BEZIER_DIVIDER;
        position = path.bezier_point(path_time);
        cam_direction = direction.bezier_point(dir_time);
        view_matrix = lookAt(position, cam_direction, vec3(0.0f, 0.0f, 1.0f));
    }


    //a matrix to do the reflection
    mat4 ref = mat4(1);
    ref[2][2]=-1;

    // create new VP for mirrored camera
    mat4 mirror_V_matrix = view_matrix * ref;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    noise_framebuffer.Bind();
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        noise.Draw(time, IDENTITY_MATRIX, view_matrix, projection_matrix);
        glReadPixels(window_width*(cam_pos[0]+1)/2,window_height*(cam_pos[1]+1)/2,1,1,GL_RED , GL_FLOAT ,&altitude);
        std::cout<<cam_pos[0]<<","<< cam_pos[1]<<","<<cam_pos[2]<<endl;}
    if(cam_mode == FIRST_PERSON_SHOOTER){cam_pos[2]=altitude+0.01;}
    noise_framebuffer.Unbind();

    water_refl.Bind();
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        if(cam_mode==DEFAULT){
            mirror_V_matrix= mirror_V_matrix;
        }
        skybox.Draw(model_matrix, mirror_V_matrix, projection_matrix);
        grid.Draw(time, model_matrix,  mirror_V_matrix, projection_matrix, NOT_DRAW_SAND);

    }
    water_refl.Unbind();

    //draw before solid objects, then transparent objects to achive the blending of the colours
    if(cam_mode==DEFAULT){
        view_matrix= view_matrix;
    }
    grid.Draw(time, model_matrix, view_matrix, projection_matrix);
    water.Draw(time, model_matrix,  view_matrix, projection_matrix);
    skybox.Draw(model_matrix, view_matrix, projection_matrix);

}

// Gets called when the windows/framebuffer is resized.
void SetupProjection(GLFWwindow* window, int width, int height) {
    window_width = width;
    window_height = height;

    cout << "Framebuffer has been resized to "
         << window_width << "x" << window_height << "." << endl;

    glViewport(0, 0, window_width, window_height);

    //Use a perspective projection instead;
    projection_matrix = PerspectiveProjection(45.0f,
                                              (GLfloat)window_width / window_height,
                                              0.0001f, 100.0f);
    noise_framebuffer.Cleanup();
    water_refl.Cleanup();
    noise_framebuffer.Init(window_width, window_height, LINEAR_INTERP);
    water_refl.Init(window_width, window_height, LINEAR_INTERP, WATER_REFLECTION );
    glClearColor(0.9, 0.9, 1.0 /*gray*/, 0.5 /*solid*/);
}

void ErrorCallback(int error, const char* description) {
    fputs(description, stderr);
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods){
    switch(key){
    case GLFW_KEY_ESCAPE :
        glfwSetWindowShouldClose(window, GL_TRUE);
        break;
    case GLFW_KEY_SPACE :
        pause = !pause;
        stop_time = (float)glfwGetTime();
        break;
        //camera mods
    case '1' :
        cam_mode = DEFAULT;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        glfwSetWindowTitle(window, "ICG Project");
        break;
    case '2' :
        cam_mode = BEZIER;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        glfwSetWindowTitle(window, "Camera mode : Bezier");
        b_path_start = true;
        bezier_speed = 1;
        bezier_time = 0;
        break;
    case '3' :
        cam_mode = FIRST_PERSON_SHOOTER;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); //cursor disabled
        glfwSetWindowTitle(window, "Camera mode : First Person Shooter");
        break;
    case '4' :
        cam_mode = FLY_THROUGH;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); //cursor disabled
        glfwSetWindowTitle(window, "Camera mode : Fly Through Mode");
        break;
    case '5' :
        cam_mode = INFINITE_TERRAIN;
        glfwSetWindowTitle(window, "Camera mode : Infinite Terrain");
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        break;
    }

    //switch case for the different camera modes.
    switch(cam_mode){
    case DEFAULT:
        break;

    case BEZIER:
        if (key == GLFW_KEY_W ) {
            if(bezier_speed < 5) {
                bezier_speed += 0.1;
            }
        }
        if (key == GLFW_KEY_S) {
            if(bezier_speed > 0.20) {
                bezier_speed -= 0.1;
            }
        }

        break;

    case FIRST_PERSON_SHOOTER:
        /* Camera is pinned to the ground, camera can move
             * around with WASD keys and the the mouse orientes
             * the camera*/
        //fps_height= noise_framebuffer.terrain_height;

        std::cout<<altitude<<endl;
        if (key == GLFW_KEY_W ) {
            cam_speed.x += 0.3*PACE;
        }
        if (key == GLFW_KEY_S) {
            cam_speed.x -= 0.3*PACE;
        }

        if (key == GLFW_KEY_A) {
            cam_speed.y -= 0.3*PACE;
        }

        if (key == GLFW_KEY_D) {
            cam_speed.y += 0.3*PACE;
        }

        break;
    case FLY_THROUGH:
        /*kind of god-mode*/
        if (key == GLFW_KEY_W ) {
            cam_speed.x += PACE;
        }
        if (key == GLFW_KEY_S) {
            cam_speed.x -= PACE;
        }

        if (key == GLFW_KEY_A) {
            cam_speed.y -= PACE;
        }

        if (key == GLFW_KEY_D) {
            cam_speed.y += PACE;
        }
        break;

    case INFINITE_TERRAIN:
        /* Camera is static, the noise moves. The camera
             * will never be near the edge of the terrain so
             * it will give the look on infinite terrain*/
        break;
    }
}

int main(int argc, char *argv[]) {
    // GLFW Initialization
    if(!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return EXIT_FAILURE;
    }

    glfwSetErrorCallback(ErrorCallback);

    // hint GLFW that we would like an OpenGL 3 context (at least)
    // http://www.glfw.org/faq.html#how-do-i-create-an-opengl-30-context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // attempt to open the window: fails if required version unavailable
    // note some Intel GPUs do not support OpenGL 3.2
    // note update the driver of your graphic card
    GLFWwindow* window = glfwCreateWindow(window_width, window_height,
                                          "Infinite Terrain", NULL, NULL);
    if(!window) {
        glfwTerminate();
        return EXIT_FAILURE;
    }
    glfwSetWindowTitle(window, "ICG Project");

    // makes the OpenGL context of window current on the calling thread
    glfwMakeContextCurrent(window);

    // set the callback for escape key
    glfwSetKeyCallback(window, KeyCallback);

    // set the framebuffer resize callback
    glfwSetFramebufferSizeCallback(window, SetupProjection);

    // set the mouse press and position callback
    glfwSetMouseButtonCallback(window, MouseButton);
    glfwSetCursorPosCallback(window, MousePos);

    // GLEW Initialization (must have a context)
    // https://www.opengl.org/wiki/OpenGL_Loading_Library
    glewExperimental = GL_TRUE; // fixes glew error (see above link)
    if(glewInit() != GLEW_NO_ERROR) {
        fprintf( stderr, "Failed to initialize GLEW\n");
        return EXIT_FAILURE;
    }

    cout << "OpenGL" << glGetString(GL_VERSION) << endl;

    // update the window size with the framebuffer size (on hidpi screens the
    // framebuffer is bigger)
    glfwGetFramebufferSize(window, &window_width, &window_height);
    //set projection matrix
    SetupProjection(window, window_width, window_height);

    // initialize our OpenGL program
    Init();

    if (cam_mode == FLY_THROUGH)
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); //cursor disabled

    // render loop
    while(!glfwWindowShouldClose(window)){
        Display(window);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    noise.Cleanup();
    skybox.Cleanup();
    noise_framebuffer.Cleanup();
    water_refl.Cleanup();
    grid.Cleanup();

    // close OpenGL window and terminate GLFW
    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;
}
