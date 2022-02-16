/*
 * spnda, Copyright (c) 2020
 * GLWindow.cpp
 */
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#include <lodepng.h>

#include <GLWindow.h>
#include <Shader.h>
#include <vector3.h>

namespace fs = std::filesystem;

void GLAPIENTRY glErrorMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message,
                                       const void* userParam) {
#ifdef _DEBUG
    std::cout << message << std::endl;
#endif
}

GLWindow::GLWindow(const char* title)
    : title(title), windowSizeCache(0, 0), windowPositionCache(0, 0) {};

int GLWindow::init() {
    if (!glfwInit())
        return -1;
    // set the window to be as big as the monitor, but don't be fullscreen.
    const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    width = mode->width;
    height = mode->height;
    windowSizeCache = vector2i(width, height);
    this->window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* handle, int width, int height) noexcept -> void {
        auto window = reinterpret_cast<GLWindow*>(glfwGetWindowUserPointer(handle));
        window->framebufferSizeCallback(handle, width, height);
    });
    glfwSetKeyCallback(window, [](GLFWwindow* handle, int key, int scancode, int action, int mods) noexcept -> void {
        auto window = reinterpret_cast<GLWindow*>(glfwGetWindowUserPointer(handle));
        window->onKey(key, scancode, action, mods);
    });
    glfwGetWindowSize(window, (int*)&windowSizeCache.x, (int*)&windowSizeCache.y);
    glfwGetWindowPos(window, (int*)&windowPositionCache.x, (int*)&windowPositionCache.y);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize OpenGL context" << std::endl;
        return -1;
    }

    if (!GLAD_GL_VERSION_4_5) {
        std::cout << "OpenGL version not sufficient. Expected 4.5, got: " << glGetString(GL_VERSION) << std::endl;
    }

    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;

    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(glErrorMessageCallback, 0);

    renderShader = std::make_unique<RasterizedProgram>();
    renderShader->create("shaders/render.vert", "shaders/render.frag");

    // get all shaders in the "shaders/" subfolder
    for (const auto& file : fs::directory_iterator("./shaders")) {
        if (file.is_regular_file()) {
            fs::path fileName = file.path();
            fs::path ext = fileName.extension();
            if (ext == ".comp") {
                ComputeProgram program;
                program.create(fileName);
                shaders.emplace_back(std::move(program));
            }
        }
    }

    // End the program if no shaders were found.
    if (shaders.size() == 0)
        return -1;

    return 0;
};

int GLWindow::render() {
    if (window == nullptr)
        return -1;
    if (shaders.size() == 0)
        return -1;

    // They're generated in the vert shader
    static const GLfloat fst[] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0,
    };

    // Fullscreen triangle vertices
    GLuint vbo;
    glCreateBuffers(1, &vbo);
    glNamedBufferData(vbo, sizeof(fst), fst, GL_STATIC_DRAW);

    // Compute texture
    GLuint computeTexture;
    glCreateTextures(GL_TEXTURE_2D, 1, &computeTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTextureStorage2D(computeTexture, 1, GL_RGBA8, width, height);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Framebuffer was incomplete!" << std::endl;
        return -1;
    }

    glfwSwapInterval(0);

    shaderIndex = 0;

    int frameCounter = 0;
    double frameTime = 0;

    double currentTime = glfwGetTime(), lastTime = 0.0;
    int fpsFrameCounter = 0;

    while (!glfwWindowShouldClose(window)) {
        auto& shader = shaders[shaderIndex];

        glClear(GL_COLOR_BUFFER_BIT);

        frameCounter++;
        fpsFrameCounter++;

        // Calculate and display FPS
        currentTime = glfwGetTime();
        double timeDelta = currentTime - lastTime;
        if (timeDelta >= 1.0) { // Update window title every second
            std::stringstream ss;
            ss << "fractal "
               << " [" << shader.name << ", " << fpsFrameCounter / timeDelta << " FPS]";
            glfwSetWindowTitle(window, ss.str().c_str());
            fpsFrameCounter = 0;
            lastTime = currentTime;
        }

        // We first use compute to draw the fractals onto an image
        shader.use();
        shader.setDouble("zoom", 1.0f);
        shader.setDouble("iTime", currentTime);
        shader.setVec2("position", (float)0.0, (float)0.0);
        shader.setVec2("iResolution", width, height);

        glBindImageTexture(0, computeTexture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);
        glDispatchCompute(width / 8, height / 8, 1);
        glMemoryBarrier(GL_TEXTURE_UPDATE_BARRIER_BIT);

        shader.disable();

        // Then we "copy" that image onto the framebuffer color attachment using a FST
        renderShader->use();
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        glBindTextureUnit(0, computeTexture);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glDisableVertexAttribArray(0);
        renderShader->disable();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void GLWindow::onKey(int key, int scancode, int action, int mods) {
    double movementAddition = 0.0005;
    switch (key) {
        case GLFW_KEY_ESCAPE: glfwSetWindowShouldClose(window, true); break;
        case GLFW_KEY_F1:
            if (action == GLFW_PRESS && shaderIndex > 0) {
                shaderIndex--;
            }
            break;
        case GLFW_KEY_F2:
            if (action == GLFW_PRESS && shaderIndex < shaders.size() - 1) {
                shaderIndex++;
            }
            break;
        case GLFW_KEY_F11:
            if (action != GLFW_PRESS)
                break;
            toggleFullscreen();
            break;
        case GLFW_KEY_F12:
            if (action != GLFW_PRESS)
                break;
            takeScreenshot();
            break;
    }
};

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

void GLWindow::framebufferSizeCallback(GLFWwindow* window, int nWidth, int nHeight) {
    this->width = nWidth;
    this->height = nHeight;
    glViewport(0, 0, nWidth, nHeight);
}

// TODO: In the future, take screenshots at a very high resolution to allow for zooming inside the image.
void GLWindow::takeScreenshot() {
    std::stringstream ss;
    ss << std::chrono::system_clock::now().time_since_epoch().count() << ".png";
    const std::string tmp = ss.str(); // tmp to avoid dangling pointer
    const char* fn = tmp.c_str();
    // read pixels
    int n = 4 * width * height;
    std::vector<GLubyte> pixels(n);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
    if (glGetError() != GL_NO_ERROR) {
        std::cout << "Error: Unable to read pixels." << std::endl;
        return;
    }
    // the image will go from top left to bottom right
    // the opengl canvas goes from bottom left to top right.
    // therefore, convert the array to a 2d vector and then flip it.
    std::vector<GLubyte> newp(n);
    for (uint32_t i = 1; i < height; i++) { // i should be the amount of lines in this image
        uint32_t off = n - (width * i * 4);
        for (uint32_t j = 0; j < width * 4; j++) {
            newp[off + j] = pixels[(width * i * 4) + j];
        }
    }

    // see https://en.wikipedia.org/wiki/Portable_Network_Graphics
    lodepng::encode(fn, newp, width, height, LCT_RGBA, 8U);
}
