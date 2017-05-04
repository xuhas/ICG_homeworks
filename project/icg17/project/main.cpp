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

Grid grid;
Noise noise;
Skybox skybox;
FrameBuffer framebuffer;
Water water;
//FrameBuffer water_reflection; // water reflection

int window_width = 800;
int window_height = 600;

using namespace glm;

mat4 projection_matrix;
mat4 view_matrix;
mat4 trackball_matrix;
mat4 old_trackball_matrix;
mat4 model_matrix;

Trackball trackball;

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

    noise.Init();
    skybox.Init();
    //GLuint framebuffer_texture_id = framebuffer.Init(512, 512);
    GLuint framebuffer_texture_id = framebuffer.Init(window_width, window_height);
    //GLuint water_reflection_tex_id = water_reflection.Init(window_width, window_height, false);

    noise.Init();
    grid.Init(framebuffer_texture_id);
    //water.Init(water_reflection_tex_id);
    water.Init(framebuffer_texture_id);


    // enable depth test.
    glEnable(GL_DEPTH_TEST);

    // once you use the trackball, you should use a view matrix that
    // looks straight down the -z axis. Otherwise the trackball's rotation gets
    // applied in a rotated coordinate frame.
    view_matrix = LookAt(vec3(0.0f, 0.0f, 40.0f),
                         vec3(0.0f, 0.0f, 0.0f),
                         vec3(0.0f, 1.0f, 0.0f));

    view_matrix = translate(view_matrix, vec3(0.0f, 0.0f, 0.0f));

    trackball_matrix = IDENTITY_MATRIX;

    model_matrix = scale(mat4(1.0f),vec3(60,60,60));
    model_matrix = translate(model_matrix,vec3(-0.25f, -0.25f, 0.0f));
}

// gets called for every frame.
void Display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const float time = glfwGetTime();
    //float water_height_sh = WATER_HEIGHT + WATER_AMPL * sin(time);
    //float water_height = (water_height_sh + 1) / 2;

    framebuffer.Bind();
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        noise.Draw(time, IDENTITY_MATRIX, view_matrix, projection_matrix);
    }
    framebuffer.Unbind();


//    water_reflection.Bind();
//    {
//        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//        glEnable(GL_CLIP_DISTANCE0);

//        drawChunks(model_matrix, view_matrix_reflection, projection_matrix, curr_time, water_height);
//        sky.Draw(translate(projection_matrix * model_matrix * view_matrix_reflection, cam_pos2));
//        drawTrees(model_matrix, view_matrix_reflection, projection_matrix, curr_time, water_height_sh);

//        glDisable(GL_CLIP_DISTANCE0);
//    }
//    water_reflection.Unbind();
    //mat4 model = scale(model_matrix, vec3(WATER_SIZE, 1, WATER_SIZE));
    //model = translate(model, vec3(cam_pos.x / WATER_SIZE, WATER_HEIGHT, cam_pos.z / WATER_SIZE));

    //draw before solid objects, then transparent objects to achive the blending of the colours
    grid.Draw(time, trackball_matrix * model_matrix, view_matrix, projection_matrix);
    water.Draw(time, trackball_matrix * model_matrix, view_matrix, projection_matrix);
    skybox.Draw(trackball_matrix * quad_model_matrix, view_matrix, projection_matrix);
    grid.Draw(time, trackball_matrix * quad_model_matrix, view_matrix, projection_matrix);


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

float last_y = 0;

void MouseButton(GLFWwindow* window, int button, int action, int mod) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double x_i, y_i;
        glfwGetCursorPos(window, &x_i, &y_i);
        vec2 p = TransformScreenCoords(window, x_i, y_i);
        trackball.BeingDrag(p.x, p.y);
        old_trackball_matrix = trackball_matrix;
        // Store the current state of the model matrix.
    }
}

void MousePos(GLFWwindow* window, double x, double y) {
    vec2 p = TransformScreenCoords(window, x, y);
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        // Calculate 'trackball_matrix' given the return value of
        // trackball.Drag(...) and the value stored in 'old_trackball_matrix'.
        // See also the mouse_button(...) function.
        //trackball_matrix = ...
        trackball_matrix =  trackball.Drag(p.x, p.y) * old_trackball_matrix;
    }

    // zoom
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        // Implement zooming. When the right mouse button is pressed,
        // moving the mouse cursor up and down (along the screen's y axis)
        // should zoom out and it. For that you have to update the current
        // 'view_matrix' with a translation along the z axis.
        // view_matrix = ...
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
                                              0.1f, 400.0f);
    framebuffer.Cleanup();
    framebuffer.Init(window_width, window_height);
}

void ErrorCallback(int error, const char* description) {
    fputs(description, stderr);
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
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

    // initialize our OpenGL program
    Init();

    // update the window size with the framebuffer size (on hidpi screens the
    // framebuffer is bigger)
    glfwGetFramebufferSize(window, &window_width, &window_height);
    SetupProjection(window, window_width, window_height);

    // render loop
    while(!glfwWindowShouldClose(window)){
        Display();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    noise.Cleanup();
    skybox.Cleanup();
    framebuffer.Cleanup();
    grid.Cleanup();

    // close OpenGL window and terminate GLFW
    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;
}
