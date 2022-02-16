/*
 * spnda, Copyright (c) 2020
 * vector3.h
 */

#pragma once

struct vector3 {
    float x, y, z;

    vector3() : x(0), y(0), z(0) {};
    vector3(float x, float y, float z) : x(x), y(y), z(z) {};
    ~vector3() {}
};
