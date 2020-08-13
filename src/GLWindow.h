/*
 * spnda, Copyright (c) 2020
 * GLWindow.h
 */
#pragma once

#include <GLFW\glfw3.h>
#include <chrono>

#include "vector2.h"

using namespace std::chrono;

class GLWindow {
public:
	GLFWwindow* window;
	
	GLWindow(int width, int height, const char* title);

	int init();
	int render();
	void onKey(int key, int scancode, int action, int mods);
	void onMouse(int button, int action, int mods);
	void onScroll(double xoffset, double yoffset);

private:
	int width, height, pixelWidth = 1;
	const char* title;
	bool fullscreen = false, zooming = true;
	double zoomFactor = 1.0, zoomFactorAdd = 0, positionX = 0.0, positionY = 0.0, movementScale = 0.05;
	double animationCounter = 0.0, animationSpeed = 0.0001;
	vector2 windowSizeCache;
	vector2 windowPositionCache;

	void framebufferSizeCallback(GLFWwindow *window, int width, int height);
	void toggleFullscreen();
};
