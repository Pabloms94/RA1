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
	Vector getOrigen() { return origen; }
	Vector getDir() { return direction; }

	void setOrigen(Vector point) { origen = point; }
	void setDir(Vector dir) { direction = dir; }
};