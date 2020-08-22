/*
 * spnda, Copyright (c) 2020
 * fractal.cpp
 */
#include "GLWindow.h"

int main(int argc, char** argv) {
	GLWindow *glWindow = new GLWindow("fractal");
	glWindow->init();
	glWindow->render();
	return 0;
}
