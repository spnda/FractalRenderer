/*
 * spnda, Copyright (c) 2020
 * GLWindow.h
 */
#pragma once

#include "Shader.h" // At the top due to glew initialization

#include <GLFW\glfw3.h>
#include <vector>

#include "vector2.h"

class GLWindow {
public:
	GLFWwindow* window;

	GLWindow(const char* title);

	int init();
	int render();
	void onKey(int key, int scancode, int action, int mods);
	void onMouse(int button, int action, int mods);
	void onScroll(double xoffset, double yoffset);

private:
	int width, height, pixelWidth = 1, shaderIndex = 0;
	const char* title;
	bool fullscreen = false, zooming = true;
	double zoomFactor = 1.0, zoomFactorAdd = 0, positionX = 0.0, positionY = 0.0, movementScale = 0.05;
	double animationCounter = 0.0, animationSpeed = 0.0001;
	vector2i windowSizeCache;
	vector2i windowPositionCache;
	std::vector<Shader> shaders;
	Shader *shader = nullptr;

	void framebufferSizeCallback(GLFWwindow *window, int width, int height);
	void toggleFullscreen();
	void takeScreenshot();
};
