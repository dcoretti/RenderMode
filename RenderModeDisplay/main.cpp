#define WIN32_LEAN_AND_MEAN
#include <iostream>
#include <string>
#include <io.h>
#include <fcntl.h>
#include <stdio.h>
#include <Windows.h>

#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#include "GL/glew.h"
#include <GLFW/glfw3.h>

#include <string>
#include <vector>

#include "Util/RenderTest.h"

using std::string;
using std::cout;
using std::cin;
using std::endl;


void setupConsole() {
    AllocConsole();
    AttachConsole(GetCurrentProcessId());
    freopen("CON", "w", stdout);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (GLFW_PRESS == action) {
        switch (key) {
        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(window, GL_TRUE);
            break;
        }
    }
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    setupConsole();


    GLFWwindow* window;
    glewExperimental = GL_TRUE;
    if (!glfwInit()) {
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_DEPTH_BITS, 16);
    // Deprecated functionality will be removed from being marked in a previous version. 
    // in 3.1 it will remove anything marked as deprecated in 3.0 but not removed in 3.1
    // This is ONLY useful between 3.0 and 3.1 for 3.2+ this does nothing as it was a deprecation path
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_FALSE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // instead of setting profile, giving hints and letting system decide
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwSwapInterval(1);	//vsync

    window = glfwCreateWindow(640, 480, "RenderMode", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);


    // some debug string info
    const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
    const GLubyte* version = glGetString(GL_VERSION); // version as a string
    cout << "Renderer " << renderer << endl;
    cout << "OpenGL version supported " << version << endl;

    //glEnable(GL_DEPTH_TEST); // enable depth-testing
    //glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
    //glEnable(GL_CULL_FACE);
    glfwSetKeyCallback(window, keyCallback);

    GLenum err = glewInit();
    if (GLEW_OK != err) {
        cout << "Error initializing glew: " << glewGetErrorString(err) << endl;
        return -1;
    }

    bool quitting = false;
    RenderTest t;
    bool didTest = false;
    unsigned int frameCounter = 0;
    cout << " MAX TEXTURES: " << GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS << endl;
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        if (!didTest) {
            //t.loadIndexCommands();
            //t.attemptLoadCommands(true);
            //t.modelLoad();
            //t.texLoad(true);
            t.doubleModel();
            didTest = true;
        }

        glClear(GL_COLOR_BUFFER_BIT);
        //t.draw();
        t.drawShapes();
        glfwSwapBuffers(window);
        frameCounter++;
        //cout << frameCounter << endl;
    }

    glfwTerminate();
    return 0;
}