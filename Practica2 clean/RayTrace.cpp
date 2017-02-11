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
   /*
   -- How to Implement a Ray Tracer --

   This computed pixel will take into account the camera and the scene
   and return a Vector of <Red, Green, Blue>, each component ranging from 0.0 to 1.0

   In order to start working on computing the color of this pixel,
   you will need to cast a ray from your current camera position
   to the image-plane at the given screenX and screenY
   coordinates and figure out how/where this ray intersects with 
   the objects in the scene descriptor.
   The Scene Class exposes all of the scene's variables for you 
   through its functions such as m_Scene.GetBackground (), m_Scene.GetNumLights (), 
   etc. so you will need to use those to learn about the World.

   To determine if your ray intersects with an object in the scene, 
   you must calculate where your objects are in 3D-space [using 
   the object's scale, rotation, and position is extra credit]
   and mathematically solving for an intersection point corresponding to that object.

   For example, for each of the spheres in the scene, you can use 
   the equation of a sphere/ellipsoid to determine whether or not 
   your ray from the camera's position to the screen-pixel intersects 
   with the object, then from the incident angle to the normal at 
   the contact, you can determine the reflected ray, and recursively 
   repeat this process capped by a number of iterations (e.g. 10).

   Using the lighting equation & texture to calculate the color at every 
   intersection point and adding its fractional amount (determined by the material)
   will get you a final color that returns to the eye at this point.
   */

	if (screenX == 50 && screenY ==100)
   {
      int kk=0;
   }
   Scene &la_escena = m_Scene;
   Camera &la_camara = la_escena.GetCamera();
   Vector posicion = la_camara.GetPosition();


   if ((screenX < 0 || screenX > Scene::WINDOW_WIDTH - 1) ||
      (screenY < 0 || screenY > Scene::WINDOW_HEIGHT - 1))
   {
      // Off the screen, return black
      return Vector (0.0f, 0.0f, 0.0f);
   }

   Ray ray = CalculateRay(la_camara, screenX, screenY);

   return ray.getDir();

   //printf("(%f, %f, %f)\n", ray.getDir().x, ray.getDir().y, ray.getDir().z);

   // Until this function is implemented, return white
   
   /*for (int i = 0; i < la_escena.GetNumObjects(); i++)
   {
	   SceneSphere esfera = *(SceneSphere)(la_escena.GetObjectW(i));
	   if (la_escena.GetObjectW(i)->IsSphere()){
		   if ((pow((ray.getDir().x - la_escena.GetObjectW(i)->position.x), 2) +
			   pow((ray.getDir().y - la_escena.GetObjectW(i)->position.y), 2) +
			   pow((ray.getDir().z - la_escena.GetObjectW(i)->position.z), 2)) > pow((SceneSphere)esfera, 2))
		   {

		   }
	   }
   }*/
   
}


Ray RayTrace::CalculateRay(Camera &cam, int screenX, int screenY){

	//Teniendo height y width, tenemos que sumárselo a forward para obtener el vector que vaya al pixel.
	Ray resultado;

	//const float aspect = Scene::WINDOW_WIDTH / Scene::WINDOW_HEIGHT;
	//float halfHeight = tanf((cam.GetFOV()) * 0.5f);

	double pixelCameraX = ((float)screenX / m_Scene.WINDOW_WIDTH)*tanf(cam.GetFOV());
	double pixelCameraY = ((float)screenY / m_Scene.WINDOW_HEIGHT)*tanf(cam.GetFOV());
	//Vector dir = Vector(pixelCameraX, pixelCameraY, -1.0f) - cam.position;

	//dir.Normalize();
	
	resultado.setDir((Vector(pixelCameraX, pixelCameraY, -1.0f) - cam.GetPosition()));// .Normalize());

	return resultado;
}