/*
 * spnda, Copyright (c) 2020
 * fractal.cpp
 */

#include <GLWindow.h>

int main(int argc, char** argv) {
    auto glWindow = std::make_unique<GLWindow>("fractal");
    if (glWindow->init() != 0)
        return -1;
    glWindow->render();
    return 0;
}
