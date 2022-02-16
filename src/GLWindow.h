/*
 * spnda, Copyright (c) 2020
 * GLWindow.h
 */
#pragma once

#include <memory>
#include <vector>

#define GLFW_INCLUDE_NONE
#include <GLFW\glfw3.h>
#include <glad/glad.h>

#include <shader.h>
#include <vector2.h>

class GLWindow {
    GLFWwindow* window = nullptr;

    uint32_t width, height, pixelWidth = 1, shaderIndex = 0;
    const char* title;
    bool fullscreen = false;
    vector2i windowSizeCache;
    vector2i windowPositionCache;

    std::vector<ComputeProgram> shaders;
    std::unique_ptr<RasterizedProgram> renderShader;

    void framebufferSizeCallback(GLFWwindow* window, int width, int height);
    void toggleFullscreen();
    void takeScreenshot();
	
public:
    GLWindow(const char* title);

    int init();
    int render();
    void onKey(int key, int scancode, int action, int mods);
    void onMouse(int button, int action, int mods);
    void onScroll(double xoffset, double yoffset);
};
