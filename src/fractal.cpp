/*
 * spnda, Copyright (c) 2020
 * fractal.cpp
 */
#include <iostream>
#include <GLFW/glfw3.h>
#include <chrono>
#include <string>
#include <sstream>
#include <iomanip>
#include <array>
#include <memory>
#include "vector3.h"
#include "GLWindow.h"

using namespace std::chrono;
using std::shared_ptr;

int main() {
	GLWindow *glWindow = new GLWindow(2560, 1440, "julia");
	glWindow->init();
	glWindow->render();
	return 0;
}
