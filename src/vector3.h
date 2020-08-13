#pragma once

#include <cmath>

struct vector3 {
	double x, y, z;

	vector3(double x, double y, double z) : x(x), y(y), z(z) {};

	~vector3() {}

	vector3 operator +(vector3 rhs) {
		return vector3(x * rhs.x, y * rhs.y, z * rhs.z);
	}

	vector3 operator *(double rhs) {
		return vector3(x * rhs, y * rhs, z * rhs);
	}

	vector3 operator /(double rhs) {
		return vector3(x / rhs, y / rhs, z / rhs);
	}

	static vector3 rgb(double ratio) {
		//we want to normalize ratio so that it fits in to 6 regions
		//where each region is 256 units long
		int normalized = int(ratio * 256 * 6);

		//find the distance to the start of the closest region
		int x = (int)std::fmod(normalized, 256);

		int red = 0, grn = 0, blu = 0;
		switch (normalized / 256) {
			case 0: red = 255;      grn = x;        blu = 0;       break;//red
			case 1: red = 255 - x;  grn = 255;      blu = 0;       break;//yellow
			case 2: red = 0;        grn = 255;      blu = x;       break;//green
			case 3: red = 0;        grn = 255 - x;  blu = 255;     break;//cyan
			case 4: red = x;        grn = 0;        blu = 255;     break;//blue
			case 5: red = 255;      grn = 0;        blu = 255 - x; break;//magenta
		}

		return vector3(red, grn, blu);
	}

	static vector3 randomColor() {
		return vector3(rand() % 255, rand() % 255, rand() % 255);
	}
};
