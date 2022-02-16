/*
 * spnda, Copyright (c) 2020
 * vector2.h
 */

#pragma once

struct vector2i {
    int x, y;

    vector2i() : x(0), y(0) {};
    vector2i(int x, int y) : x(x), y(y) {};
    ~vector2i() {}
};

struct vector2f {
    float x, y;

    vector2f() : x(0), y(0) {};
    vector2f(float x, float y) : x(x), y(y) {};
    ~vector2f() {};
};
