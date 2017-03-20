#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <fstream>

#include "Utils.h"

class Ray{
private:
	Vector origen;
	Vector direction;
public:
	Ray(){}
	Ray(Vector orig, Vector dir){
		this->origen = orig;
		this->direction = dir;
	}
	inline Vector getOrigen() { return origen; }
	inline Vector getDir() { return direction; }

	inline void setOrigen(Vector point) { origen = point; }
	inline void setDir(Vector dir) { direction = dir; }
};