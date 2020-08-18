/*
 * spnda, Copyright (c) 2020
 * fractal.cpp
 */
#include "GLWindow.h"

int main() {
	GLWindow *glWindow = new GLWindow(2560, 1440, "fractal");
	glWindow->init();
	glWindow->render();
	return 0;
}
