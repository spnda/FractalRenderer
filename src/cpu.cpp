#include <iostream>
#include <iomanip>
#include <GLFW/glfw3.h>
#include <chrono>

#include "vector3.h"

using namespace std::chrono;

double FractalRendererCalc(double x, double y) {
	int limit = 100;
	double x1 = x;
	double y1 = y;

	for (double i = 0; i < limit; i += 0.5) {
		double r2 = x1 * x1;
		double i2 = y1 * y1;

		if (r2 + i2 > 4.0) return i;

		y1 = 2.0 * x1 * y1 + y;
		x1 = r2 - i2 + x;
	}
	return limit;
}

// THIS DOES NOT WORK!
void render() {
	GLFWwindow *window;

	double positionX, positionY;

	int width, height;
	int frameCount = 0;
	int pixelWidth = 1.0;
	double frameTime = 0;
	bool zooming = false;
	double zoomFactor = 0.0;

	double x_start = positionX - 1.5 * zoomFactor;
	double x_fin = positionX + 1.5 * zoomFactor;
	double y_start = positionY - zoomFactor;
	double y_fin = positionY + zoomFactor;

	float time = 0.0;
	while (!glfwWindowShouldClose(0)) {
		glClear(GL_COLOR_BUFFER_BIT);
		auto begin = high_resolution_clock::now();

		frameCount++;
		glfwGetWindowSize(window, &width, &height);
		glClear(GL_COLOR_BUFFER_BIT);
		glPointSize(pixelWidth);
		glBegin(GL_POINTS);

		std::cout << std::fixed << std::setprecision(2) << frameTime << " ms" << std::endl;

		if (zooming) zoomFactor = zoomFactor / 1.1;

		x_start = positionX - 1.5 * zoomFactor;
		x_fin = positionX + 1.5 * zoomFactor;
		y_start = positionY - zoomFactor;
		y_fin = positionY + zoomFactor;

		double dx = (x_fin - x_start) / (width - 1);
		double dy = (y_fin - y_start) / (height - 1);

		for (float i = 0; i < width; i += pixelWidth) {
			for (float j = 0; j < height; j += pixelWidth) {
				double x = x_start + i * dx; // current real value
				double y = y_fin - j * dy; // current imaginary value
				double ret = FractalRendererCalc(x, y);
				vector3 colour = vector3::rgb(ret / 100); // <- colour
				//vector3 color = vector3(255, 255, 255) * (ret / 100); // <- black and white
				glColor3f(colour.x / 255, colour.y / 255, colour.z / 255);
				glVertex2d(i, j);
			}
		}
		glEnd();
		glFlush();

		frameTime = std::chrono::duration_cast<std::chrono::milliseconds>(high_resolution_clock::now() - begin).count();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}