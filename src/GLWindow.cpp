/*
 * spnda, Copyright (c) 2020
 * GLWindow.cpp
 */
#include <iostream>
#include <iomanip>

#include "Shader.h"
#include "GLWindow.h"
#include "vector3.h"

GLWindow::GLWindow(int width, int height, const char* title) : width(width), height(height), title(title) {
	windowSizeCache = *new vector2i(width, height);
	windowPositionCache = *new vector2i(0, 0);
};

int GLWindow::init() {
	if (!glfwInit()) return -1;
	this->window = glfwCreateWindow(width, height, title, nullptr, nullptr);
	if (!window) {
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetWindowUserPointer(window, &window);
	glfwSetFramebufferSizeCallback(window, [](GLFWwindow *handle, int width, int height) noexcept -> void {
		auto& window = *reinterpret_cast<GLWindow*>(glfwGetWindowUserPointer(handle));
		window.framebufferSizeCallback(handle, width, height);
	});
	glfwSetMouseButtonCallback(window, [](GLFWwindow* handle, int button, int action, int mods) noexcept -> void {
		auto& window = *reinterpret_cast<GLWindow*>(glfwGetWindowUserPointer(handle));
		window.onMouse(button, action, mods);
	});
	glfwSetScrollCallback(window, [](GLFWwindow* handle, double xoffset, double yoffset) noexcept -> void {
		auto& window = *reinterpret_cast<GLWindow*>(glfwGetWindowUserPointer(handle));
		window.onScroll(xoffset, yoffset);
	});
	glfwSetKeyCallback(window, [](GLFWwindow *handle, int key, int scancode, int action, int mods) noexcept -> void {
		auto& window = *reinterpret_cast<GLWindow*>(glfwGetWindowUserPointer(handle));
		window.onKey(key, scancode, action, mods);
	});
	glfwGetWindowSize(window, (int*)&windowSizeCache.x, (int*)&windowSizeCache.y);
	glfwGetWindowPos(window, (int*)&windowPositionCache.x, (int*)&windowPositionCache.y);

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) return -1;

	std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
	std::cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
	std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
	std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
};

int GLWindow::render() {
	if (window == nullptr) return -1;

	static const GLfloat vertexData[] = {
		-1.0f, -1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		1.0f, -1.0f, 0.0f
	};

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	GLuint vertexBuffer;
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);

	Shader shader(".\\shaders\\julia_animiert.vert", ".\\shaders\\julia_animiert.frag");

	float time = 0.0;
	int frameCounter = 0;
	double frameTime = 0;

	while (!glfwWindowShouldClose(window)) {
		std::cout << std::fixed << std::setprecision(1) << frameTime << " ms  " << '\r';
		auto begin = high_resolution_clock::now();

		glClear(GL_COLOR_BUFFER_BIT);

		frameCounter++;

		if (animationCounter < 2.0 && frameCounter % 2 == 0) animationCounter += (double)animationSpeed;
		else if (frameCounter % 2 == 0) animationCounter = (double)0.0;

		if (zooming && zoomFactor - zoomFactorAdd > 0.0) zoomFactor -= zoomFactorAdd;

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
		shader.use();
		shader.setFloat("time", time);
		shader.setDouble("zoom", zoomFactor);
		shader.setDouble("animationCounter", animationCounter);
		shader.setDouble("positionX", positionX);
		shader.setDouble("positionY", positionY);
		shader.setInt("height", height);
		shader.setInt("width", width);
		glDrawArrays(GL_TRIANGLES, 0, 4);
		glDisableVertexAttribArray(0);
		shader.disable();

		glfwSwapBuffers(window);
		glfwPollEvents();

		frameTime = std::chrono::duration_cast<std::chrono::milliseconds>(high_resolution_clock::now() - begin).count();
	}

	glfwTerminate();
	return 0;
}

void GLWindow::onKey(int key, int scancode, int action, int mods) {
	double movementAddition = 0.0005;
	switch (key) {
		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(window, true);
			break;
		case GLFW_KEY_RIGHT:
			positionX += movementScale;
			break;
		case GLFW_KEY_LEFT:
			positionX -= movementScale;
			break;
		case GLFW_KEY_DOWN:
			positionY -= movementScale;
			break;
		case GLFW_KEY_UP:
			positionY += movementScale;
			break;
		case GLFW_KEY_F5:
			// reset
			if (action == GLFW_PRESS) break;
			zoomFactor = 1.0;
			positionX = positionY = 0.0;
			break;
		case GLFW_KEY_F6:
			// zoom in
			if (action != GLFW_PRESS) break;
			if (zoomFactorAdd != 0.009) zoomFactorAdd = 0.009;
			else zoomFactorAdd = 0.0;
			break;
		case GLFW_KEY_F7:
			// zoom out
			if (action == GLFW_PRESS) break;
			if (zoomFactorAdd != -0.009) zoomFactorAdd = -0.009;
			else zoomFactorAdd = 0.0;
			break;
		case GLFW_KEY_F9:
			if (action != GLFW_PRESS) break;
			if (animationSpeed - 0.001 > 0.0) animationSpeed -= 0.001;
			break;
		case GLFW_KEY_F10:
			if (action != GLFW_PRESS) break;
			if (animationSpeed + 0.001 < 0.1) animationSpeed += 0.001;
			break;
		case GLFW_KEY_F11:
			if (action != GLFW_PRESS) break;
			toggleFullscreen();
			break;
	}
};

void GLWindow::onMouse(int button, int action, int mods) {
	switch (button) {
		case GLFW_MOUSE_BUTTON_LEFT:
			if (action == GLFW_PRESS) {
				double x, y;
				glfwGetCursorPos(window, &x, &y);
				//positionX += (x / width * 4.0 - 2.0) * zoomFactor;
				//positionY += (y / height * 4.0 - 2.0) * zoomFactor;
			}
			break;
	}
}

void GLWindow::onScroll(double xoffset, double yoffset) {
	zoomFactorAdd += yoffset / height;
}

void GLWindow::toggleFullscreen() {
	if (fullscreen) {
		fullscreen = false;
		glfwSetWindowMonitor(window, nullptr, windowPositionCache.x, windowPositionCache.y, windowSizeCache.x, windowSizeCache.y, 60);
	} else {
		fullscreen = true;
		glfwGetWindowSize(window, (int*)&windowSizeCache.x, (int*)&windowSizeCache.y);
		glfwGetWindowPos(window, (int*)&windowPositionCache.x, (int*)&windowPositionCache.y);
		const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
		glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, mode->width, mode->height, 60);
	}
}

void GLWindow::framebufferSizeCallback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}
