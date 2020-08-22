/*
 * spnda, Copyright (c) 2020
 * GLWindow.cpp
 */
#include <iomanip>
#include <string>
#include <iostream>
#include <filesystem>
#include <lodepng.h>

#include "Shader.h"
#include "GLWindow.h"
#include "vector3.h"

namespace fs = std::filesystem;

GLWindow::GLWindow(const char* title) : title(title) {
	windowSizeCache = *new vector2i(0, 0);
	windowPositionCache = *new vector2i(0, 0);
};

int GLWindow::init() {
	if (!glfwInit()) return -1;
	// set the window to be as big as the monitor, but don't be fullscreen.
	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	width = mode->width; height = mode->height;
	windowSizeCache = *new vector2i(width, height);
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

	// get all shaders in the "shaders/" subfolder
	for (const auto& file : fs::directory_iterator("./shaders")) {
		if (file.is_regular_file()) {
			fs::path fileName = file.path();
			fs::path ext = fileName.extension();
			if (ext == ".frag" && fs::exists(fs::path(fileName).replace_extension(".vert"))) {
				Shader temp = *new Shader(fileName.replace_extension(".vert").string().c_str(), fileName.string().c_str());
				temp.name = fileName.filename().replace_extension("").string();
				shaders.push_back(temp);
			}
		}
	}
};

int GLWindow::render() {
	if (window == nullptr) return -1;
	if (shaders.size() == 0) return -1;

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
	glfwSwapInterval(0);

	shader = &shaders[shaderIndex];

	int frameCounter = 0;
	double frameTime = 0;

	double currentTime = glfwGetTime(), lastTime = 0.0;
	int fpsFrameCounter = 0;

	while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT);

		frameCounter++;
		fpsFrameCounter++;

		// Calculate and display FPS
		currentTime = glfwGetTime();
		double timeDelta = currentTime - lastTime;
		if (timeDelta >= 1.0) { // Update window title every second
			std::stringstream ss;
			ss << "fractal " << " [" << shader->name << ", " << fpsFrameCounter / timeDelta << " FPS]";
			glfwSetWindowTitle(window, ss.str().c_str());
			fpsFrameCounter = 0;
			lastTime = currentTime;
		}

		if (animationCounter < 2.0 && frameCounter % 2 == 0) animationCounter += (double)animationSpeed;
		else if (frameCounter % 2 == 0) animationCounter = (double)0.0;

		if (zooming && zoomFactor - zoomFactorAdd > 0.0) zoomFactor -= zoomFactorAdd;

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
		shader->use();
		shader->setDouble("zoom", zoomFactor);
		shader->setDouble("animationCounter", animationCounter);
		shader->setDouble("positionX", positionX);
		shader->setDouble("positionY", positionY);
		shader->setInt("height", height);
		shader->setInt("width", width);
		glDrawArrays(GL_TRIANGLES, 0, 4);
		glDisableVertexAttribArray(0);
		shader->disable();

		glfwSwapBuffers(window);
		glfwPollEvents();
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
		case GLFW_KEY_F1:
			if (action == GLFW_PRESS && shaderIndex - 1 >= 0) {
				shaderIndex--;
				shader = &shaders[shaderIndex];
				std::stringstream ss;
				ss << "fractal " << " [" << shader->name << ", 0 FPS]";
				glfwSetWindowTitle(window, ss.str().c_str());
			}
			break;
		case GLFW_KEY_F2:
			if (action == GLFW_PRESS && shaderIndex + 1 < shaders.size()) {
				shaderIndex++;
				shader = &shaders[shaderIndex];
				std::stringstream ss;
				ss << "fractal " << " [" << shader->name << ", 0 FPS]";
				glfwSetWindowTitle(window, ss.str().c_str());
			}
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
		case GLFW_KEY_F12:
			if (action != GLFW_PRESS) break;
			takeScreenshot();
			break;
	}
};

void GLWindow::onMouse(int button, int action, int mods) {
	switch (button) {
		case GLFW_MOUSE_BUTTON_LEFT:
			if (action == GLFW_PRESS) {
				//double x, y;
				//glfwGetCursorPos(window, &x, &y);
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

// TODO: In the future, take screenshots at a very high resolution to allow for zooming inside the image.
void GLWindow::takeScreenshot() {
	std::stringstream ss;
	ss << std::chrono::system_clock::now().time_since_epoch().count() << ".png";
	const std::string tmp = ss.str(); // tmp to avoid dangling pointer
	const char* fn = tmp.c_str();
	// read pixels
	int n = 4 * width * height;
	GLubyte *pixels = new GLubyte[n];
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	if (glGetError() != GL_NO_ERROR) {
		std::cout << "Error: Unable to read pixels." << std::endl;
		return;
	}
	// the image will go from top left to bottom right
	// the opengl canvas goes from bottom left to top right.
	// therefore, convert the array to a 2d vector and then flip it.
	unsigned char *newp = new unsigned char[n];
	for (int i = 1; i < height; i++) { // i should be the amount of lines in this image
		int off = n - (width * i * 4);
		for (int j = 0; j < width * 4; j++) {
			newp[off + j] = pixels[(width * i * 4) + j];
		}
	}
	
	// see https://en.wikipedia.org/wiki/Portable_Network_Graphics
	lodepng::encode(fn, newp, width, height, LCT_RGBA, 8U);
}
