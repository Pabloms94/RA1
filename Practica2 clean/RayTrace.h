/*
  15-462 Computer Graphics I
  Assignment 3: Ray Tracer
  C++ RayTracer Class
  Author: rtark
  Oct 2007

  NOTE: This is the file you will need to begin working with
		You will need to implement the RayTrace::CalculatePixel () function

  This file defines the following:
	RayTrace Class
*/
#pragma once

#include <stdio.h>
#include <stdlib.h>

#include "Utils.h"
#include "Ray.h"

/*
	RayTrace Class - The class containing the function you will need to implement

	This is the class with the function you need to implement
*/





class RayTrace
{
private:
	Camera mycam;
	Vector pos, lookat, up, right;
	float angle, tangen, aspect;
public:
	/* - Scene Variable for the Scene Definition - */
	Scene m_Scene;

	// -- Constructors & Destructors --
	RayTrace (void) {}
	~RayTrace (void) {}

	// -- Main Functions --
	// - CalculatePixel - Returns the Computed Pixel for that screen coordinate
   Vector CalculatePixel (int screenX, int screenY);
   Ray CalculateRay(int screenX, int screenY);
   Vector Barycentric(Vector p, Vector a, Vector b, Vector c);
   Vector DrawRay(Ray ray, int rebotes, Scene &la_escena, int idCollision);
   Vector Reflection(Vector dir, Vector normal);
   bool SphereCollision(SceneSphere &esfera, Ray ray, Vector &posIntersect);
   bool TriangleCollision(SceneTriangle &triangle, Ray ray, Vector &intPoint);
   Vector SphereIntersect(Ray ray, SceneSphere &esfera);
   Vector SphereColor(Scene &escena, SceneSphere sphere, Vector point);
   Vector TriangleColor(Scene &escena, SceneTriangle &triangle, Vector &point, float &u, float &v, float &w);
   bool IsCastShadow(Vector orig, Vector pLight, Scene &la_escena);
   float GetDistance(Vector origen, Vector destino);

	Vector	CalculatePixelAntialiasing(int screenX, int screenY);
	Ray		*Calculate4Rays(int screenX, int screenY);
};

