/*
 * spnda, Copyright (c) 2020
 * vector3.h
 */
#pragma once

struct vector3 {
	double x, y, z;

	vector3() : x(0), y(0), z(0) {};
	vector3(double x, double y, double z) : x(x), y(y), z(z) {};
	~vector3() {}
};
