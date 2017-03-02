#ifdef _OS_X_
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>	

#elif defined(WIN32)
#include <windows.h>
#include "GL/gl.h"
#include "GL/glu.h"
#include "GL/glut.h"

#else
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#endif



#include "Scene.h"
#include "RayTrace.h"

// -- Main Functions --
// - CalculatePixel - Returns the Computed Pixel for that screen coordinate
Vector RayTrace::CalculatePixel (int screenX, int screenY)
{
	if (screenX == 50 && screenY ==100)
   {
      int kk=0;
   }
   Scene &la_escena = m_Scene;
   Camera mycam = la_escena.GetCamera();
   Vector posicion = mycam.GetPosition();


   if ((screenX < 0 || screenX > Scene::WINDOW_WIDTH - 1) ||
      (screenY < 0 || screenY > Scene::WINDOW_HEIGHT - 1))
   {
      // Off the screen, return black
      return Vector (0.0f, 0.0f, 0.0f);
   }

   pos = mycam.GetPosition();
   lookat = mycam.GetTarget() - pos;
   up = mycam.GetUp();
   right = lookat.Cross(up);
   up = right.Cross(lookat);

   up.Normalize();
   right.Normalize();
   lookat.Normalize();

   angle = mycam.GetFOV() * 3.1416f / 180.0f;
   tangen = tan(angle / 2.0f);
   aspect = (float)Scene::WINDOW_WIDTH / (float)Scene::WINDOW_HEIGHT;

   Ray ray = CalculateRay(screenX, screenY);

   //printf("(%f, %f, %f)\n", ray.getDir().x, ray.getDir().y, ray.getDir().z);

   // Until this function is implemented, return white
   bool isIntersect = false;
   for (int i = 0; i < la_escena.GetNumObjects(); i++)
   {
		if (la_escena.GetObject(i)->IsSphere())
		{
			if (SphereCollision(*(SceneSphere*)la_escena.GetObject(i), ray))
				return Vector(1.0, 0.0, 0.0);

		}
		else
		{
			if(TriangleCollision(*(SceneTriangle*)la_escena.GetObject(i), ray))
				return Vector(0.0, 1.0, 0.0);
		}
	}
	
	return Vector(0.0, 0.0, 0.0);
   
   //return ray.getDir();
}

Ray RayTrace::CalculateRay(int screenX, int screenY){
	float pixelCameraX = ((float)screenX * 2.0f / (float)m_Scene.WINDOW_WIDTH) - 1.0f;
	float pixelCameraY = ((float)screenY * 2.0f / (float)m_Scene.WINDOW_HEIGHT) - 1.0f;

	Vector dir;
	up = right.Cross(lookat);
	dir = lookat + right * aspect * tangen * pixelCameraX + up * tangen * pixelCameraY;
	dir.Normalize();

	Ray resultado;

	resultado.setDir(dir);
	resultado.setOrigen(pos);

	return resultado;
}

bool SphereCollision(SceneSphere &esfera, Ray ray){
	float t = (esfera.center - ray.getOrigen()).Dot(ray.getDir());
	Vector proj = ray.getOrigen() + ray.getDir() * t;
	Vector dist = (proj - esfera.center);

	if (dist.Magnitude() < esfera.radius)
		return true;
	//return Vector(1.0, 0.0, 0.0);
	else
		return false;
		//return Vector(0.0, 0.0, 0.0);
}

bool TriangleCollision(SceneTriangle &triangle, Ray ray)
{
	//Get Edges
	Vector edge1 = triangle.vertex[1] - triangle.vertex[0];
	Vector edge2 = triangle.vertex[2] - triangle.vertex[0];

	Vector s1 = ray.getDir().Cross(edge2);
	float divisor = edge1.Dot(s1);

	if (divisor == 0.0f)
		return false;
		//return Vector(0.0, 0.0, 1.0);

	float invDiv = 1.0f / divisor;
	
	//First barycentric position coordinate
	Vector dir = ray.getOrigen() - triangle.vertex[0];
	float bary1 = (dir.Dot(s1)) * invDiv;

	if (bary1 < 0.0f || bary1 > 1.0f)
		return false;
		//return Vector(0.0, 0.0, 1.0);

	Vector s2 = ray.getDir().Cross(edge1);
	float bary2 = ray.getDir().Dot(s2) * invDiv;

	if (bary2 < 0.0f || (bary1 + bary2) > 1.0f)
		return false;
		//return Vector(0.0, 0.0, 1.0);

	return true;
	//return Vector(0.0, 1.0, 0.0);
}