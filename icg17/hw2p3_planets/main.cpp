// glew must be before glfw
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// contains helper functions such as shader compiler
#include "icg_helper.h"

#include "quad/quad.h"
#include <iostream>

// Quad stuff1;
// ...

Quad sun;
Quad earth;
Quad moon;
float speed=10;

void Init() {
    // sets background color
    glClearColor(1.0,1.0,1.0 /*white*/, 1.0 /*solid*/);
    sun.Init("sun.tga");
    earth.Init("earth.tga");
    moon.Init("moon.tga");
}

void Display() {
    glClear(GL_COLOR_BUFFER_BIT);

	float time_s = speed * glfwGetTime();

    //SUN EQUATIONS
	glm::mat4 S = glm::scale(glm::mat4(1.0f), glm::vec3(0.15f, 0.15f, 0.0f));
	glm::mat4 R = glm::rotate(glm::mat4(1.0f), time_s/26, glm::vec3(0.0f, 0.0f, 1.0f));
    glm::mat4 T = glm::translate(glm::mat4(1.0f), glm::vec3(0.2f, 0.0f, 0.0f));

    glm::mat4 mat = T*R*S;
    sun.Draw(mat);

    //EARTH EQUATIONS
    float a = 0.8;
    float b = 0.5;
	float x = a * cos(time_s/365);
	float y = -b * sin(time_s/365);

    glm::mat4 S_earth = glm::scale(glm::mat4(1.0f), glm::vec3(0.07f, 0.07f, 0.0f));
	glm::mat4 R_earth = glm::rotate(glm::mat4(1.0f), time_s, glm::vec3(0.0f, 0.0f, 1.0f));
    glm::mat4 T_earth = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, 0.0f));

    glm::mat4 mat_earth = T_earth*R_earth*S_earth;
    earth.Draw(mat_earth);

    //MOON EQUATIONS
	float u = 0.15 * cos(time_s/28) + x;
	float v = 0.15 * sin(time_s/28) + y;

    glm::mat4 S_moon = glm::scale(glm::mat4(1.0f), glm::vec3(0.04f, 0.04f, 0.0f));
	glm::mat4 R_moon = glm::rotate(glm::mat4(1.0f), time_s/28, glm::vec3(0.0f, 0.0f, 1.0f));
    glm::mat4 T_moon = glm::translate(glm::mat4(1.0f), glm::vec3(u, v, 0.0f));

    glm::mat4 mat_moon = T_moon*R_moon*S_moon;
    moon.Draw(mat_moon);
}

void ErrorCallback(int error, const char* description) {
    fputs(description, stderr);
}

//use up and down arrows to increase or decrase the speed
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
	if ((key == GLFW_KEY_UP && action == GLFW_PRESS) || (key == GLFW_KEY_UP && action == GLFW_REPEAT)){
		speed=speed+0.5;
		std::cout << "Speed: " << speed << std::endl;
	}
	if ((key == GLFW_KEY_DOWN && action == GLFW_PRESS) || (key == GLFW_KEY_DOWN && action == GLFW_REPEAT)){
		speed=speed-0.5;
		if (speed<0.5)
			speed=0.5;
		std::cout << "Speed: " << speed << std::endl;
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
    GLFWwindow* window = glfwCreateWindow(512, 512, "planets", NULL, NULL);
    if(!window) {
        glfwTerminate();
        return EXIT_FAILURE;
    }

    // makes the OpenGL context of window current on the calling thread
    glfwMakeContextCurrent(window);

    // set the callback for escape key
    glfwSetKeyCallback(window, KeyCallback);

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

    // render loop
    while(!glfwWindowShouldClose(window)) {
        Display();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // {stuff}.Cleanup()

    // close OpenGL window and terminate GLFW
    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;
}
