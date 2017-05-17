// glew must be before glfw
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// contains helper functions such as shader compiler
#include "icg_helper.h"

#include <glm/gtc/matrix_transform.hpp>

#include "grid/grid.h"
#include "noise/noise.h"
#include "skybox/skybox.h"
#include "framebuffer.h"
#include "trackball.h"
#include "water/water.h"
#include "param.h"

#define WATER_REFLECTION 1
#define NOT_DRAW_SAND false


Grid grid;
Noise noise;
Skybox skybox;
FrameBuffer noise_framebuffer;
FrameBuffer water_refl;
Water water;
Trackball trackball;

int window_width = 800;
int window_height = 600;

using namespace glm;

mat4 projection_matrix;
mat4 view_matrix;
mat4 trackball_matrix;
mat4 old_trackball_matrix;
mat4 model_matrix;
vec3 cam_pos;

#include "glm/ext.hpp"

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

void Init() {
    // sets background color
    glClearColor(0.9, 0.9, 1.0 /*gray*/, 0.5 /*solid*/);

    skybox.Init();
    noise.Init();
    GLuint framebuffer_noise_id = noise_framebuffer.Init(window_width, window_height,true);
    GLuint framebuffer_water_refl_id = water_refl.Init(window_width,window_height,false, WATER_REFLECTION);
    grid.Init(framebuffer_noise_id);
    water.Init(framebuffer_water_refl_id);

    // enable depth test.
    glEnable(GL_DEPTH_TEST);

    // once you use the trackball, you should use a view matrix that
    // looks straight down the -z axis. Otherwise the trackball's rotation gets
    // applied in a rotated coordinate frame.
    // For this reason we apply all the transformations to the model matrix (otherwise we couldn't be able anymore to use the trackball)
    cam_pos = vec3(0.0f, 0.0f, 1.0f);
    view_matrix = LookAt(cam_pos,
                         vec3(0.0f, 0.0f, 0.0f),
                         vec3(0.0f, 1.0f, 0.0f));

    //view_matrix = translate(view_matrix, vec3(0.0f, 0.0f, 0.0f)); //use the z axis translation if u want to zoom in/out

    trackball_matrix = IDENTITY_MATRIX;

    model_matrix = scale(mat4(1.0f),vec3(1,1,1));
    //model_matrix = rotate(model_matrix, -2.3f, vec3(0.0f, 0.0f, 1.0f) /*rot_axe*/); //now the terrain comes against the camera
    //model_matrix = rotate(model_matrix, 0.7f, vec3(1.0f, -1.0f, 0.0f) /*rot_axe*/); //now the terrain is inclinated
    //model_matrix = translate(model_matrix,vec3(-0.1f, -0.1f, 0.0f));
}

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

// gets called for every frame.
void Display() {
    //calculate the FPS
    float time = (float)glfwGetTime();
    calculate_fps(time);

    view_matrix = lookAt(cam_pos, vec3(0.0f,0.0f,0.0f), vec3(0.0f,1.0f,0.0f));

    //a matrix to do the reflection
    mat4 ref = mat4(0);
    ref[0][0]=-1; ref[1][1]=1; ref[2][2]=1; ref[3][3]=1;

    // mirror the camera position
    vec3 mirror_cam_pos(cam_pos.x, cam_pos.y, -cam_pos.z);
    // create new VP for mirrored camera
    mat4 mirror_V_matrix = lookAt(mirror_cam_pos, vec3(0.0f,0.0f,0.0f), vec3(0.0f,1.0f,0.0f)) * ref;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // when we will implement the camera movement, we will need to compute in every frame
    // the camera position.... by now cam_pos is always the same (it is the model that is moving)
    //vec3 cam_pos_water_ref = vec3(cam_pos.x, cam_pos.y, -cam_pos.z);
    //mat4 view_mat_refl = lookAt(cam_pos_water_ref, vec3(0,0,0), vec3(0,-1,0));

    noise_framebuffer.Bind();
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        noise.Draw(time, IDENTITY_MATRIX, view_matrix, projection_matrix);
    }
    noise_framebuffer.Unbind();

    water_refl.Bind();
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        skybox.Draw(trackball_matrix * model_matrix, mirror_V_matrix, projection_matrix);
        grid.Draw(time, trackball_matrix * model_matrix, mirror_V_matrix, projection_matrix, NOT_DRAW_SAND);
        //water.Draw(time, trackball_matrix * model_matrix, view_matrix, projection_matrix);

    }
    water_refl.Unbind();

    //draw before solid objects, then transparent objects to achive the blending of the colours
    grid.Draw(time, trackball_matrix * model_matrix, view_matrix, projection_matrix);
    water.Draw(time, trackball_matrix * model_matrix, view_matrix, projection_matrix);
    skybox.Draw(trackball_matrix * model_matrix, view_matrix, projection_matrix);

}

// transforms glfw screen coordinates into normalized OpenGL coordinates.
vec2 TransformScreenCoords(GLFWwindow* window, int x, int y) {
    // the framebuffer and the window doesn't necessarily have the same size
    // i.e. hidpi screens. so we need to get the correct one
    int width;
    int height;
    glfwGetWindowSize(window, &width, &height);
    return vec2(2.0f * (float)x / width - 1.0f,
                1.0f - 2.0f * (float)y / height);
}

void MouseButton(GLFWwindow* window, int button, int action, int mod) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double x_i, y_i;
        glfwGetCursorPos(window, &x_i, &y_i);
        vec2 p = TransformScreenCoords(window, x_i, y_i);
        trackball.BeingDrag(p.x, p.y);
        old_trackball_matrix = trackball_matrix;
        // Store the current state of the model matrix.

//        //print the matrixes to see how the trackball is affecting the world
//        cout<< "view matrix:" <<std::endl;
//        cout<<glm::to_string(view_matrix)<<std::endl;
//        cout<< "trackball matrix:" <<std::endl;
//        cout<<glm::to_string(trackball_matrix)<<std::endl;
    }
}

void MousePos(GLFWwindow* window, double x, double y) {
    static float last_y = 0;
    vec2 p = TransformScreenCoords(window, x, y);
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        // Calculate 'trackball_matrix' given the return value of
        // trackball.Drag(...) and the value stored in 'old_trackball_matrix'.
        // See also the mouse_button(...) function.
        trackball_matrix =  trackball.Drag(p.x, p.y) * old_trackball_matrix;
    }

    // zoom
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        // Implement zooming. When the right mouse button is pressed,
        // moving the mouse cursor up and down (along the screen's y axis)
        // should zoom out and it. For that you have to update the current
        // 'view_matrix' with a translation along the z axis.
        float var;
        var = p.y - last_y;
        view_matrix = translate(view_matrix, vec3(0.0f, 0.0f, var * ZOOM_SENSITIVITY));
    }
    last_y = p.y;
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
                                              0.1f, 100.0f);
    noise_framebuffer.Cleanup();
    water_refl.Cleanup();
    water_refl.Init(window_width, window_height);
    noise_framebuffer.Init(window_width, window_height);
}

void ErrorCallback(int error, const char* description) {
    fputs(description, stderr);
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    static float total_x = 0; //track the total movement along the axis using keyboard
    static float total_y = 0;
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    if (key == GLFW_KEY_X && action == GLFW_PRESS) { //move along X axe INCREASE
        model_matrix = translate(model_matrix,vec3(0.1f, 0.0f, 0.0f));
        total_x += 0.1f;
    }
    if (key == GLFW_KEY_C && action == GLFW_PRESS) { //move along X axe DECREASE
        model_matrix = translate(model_matrix,vec3(-0.1f, 0.0f, 0.0f));
        total_x -= 0.1f;
    }

    if (key == GLFW_KEY_Y && action == GLFW_PRESS) { //move along Y axe
        model_matrix = translate(model_matrix,vec3(0.0f, 0.1f, 0.0f)); //INCREASE
        total_y += 0.1f;
    }
    if (key == GLFW_KEY_U && action == GLFW_PRESS) { //move along Y axe
        model_matrix = translate(model_matrix,vec3(0.0f, -0.1f, 0.0f)); //DECREASE
        total_y -= 0.1f;
    }

    //cout << "total x: " << total_x << endl;
    //cout << "total y: " << total_y << endl;

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

    // render loop
    while(!glfwWindowShouldClose(window)){
        Display();
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
