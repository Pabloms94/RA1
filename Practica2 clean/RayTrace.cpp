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
   SceneSphere sphere;
   SceneTriangle triangle;
   SceneModel model;
   Vector posI;
   // Until this function is implemented, return white
   bool isIntersect = false;
   for (int i = 0; i < la_escena.GetNumObjects(); i++)
   {
		if (la_escena.GetObject(i)->IsSphere())
		{
			//printf("ENTRO1?\n");
			if (SphereCollision(*(SceneSphere*)la_escena.GetObject(i), ray, posI)){
				sphere = (*(SceneSphere*)la_escena.GetObject(i));
				Vector color = SphereColor(la_escena, sphere, posI);
				return color;
			}
		}
		else if (la_escena.GetObject(i)->IsTriangle())
		{
			if(TriangleCollision(*(SceneTriangle*)la_escena.GetObject(i), ray))
				return Vector(0.0, 1.0, 0.0);
		}
		
		else if (la_escena.GetObject(i)->IsModel()){
			
			unsigned int nTriangles = (*(SceneModel*)la_escena.GetObject(i)).GetNumTriangles();
			
			for (int j = 0; j < nTriangles; j++){
				
				//SceneTriangle *Triangulo = (*(SceneModel*)la_escena.GetObject(i)).GetTriangle(j); 
				
				//printf("(%f, %f, %f) - ", Triangulo->vertex[0].x, Triangulo->vertex[0].y, Triangulo->vertex[0].z);
				//printf("(%f, %f, %f) - ", Triangulo->vertex[1].x, Triangulo->vertex[1].y, Triangulo->vertex[1].z);
				//printf("(%f, %f, %f)\n", Triangulo->vertex[2].x, Triangulo->vertex[20].y, Triangulo->vertex[2].z);
				if (TriangleCollision(*(*(SceneModel*)la_escena.GetObject(i)).GetTriangle(j), ray)) {

					//printf("colisión!!!!\n");
					return Vector(0.0, 1.0, 1.0);
				}
					
			}
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

bool SphereCollision(SceneSphere &esfera, Ray ray, Vector &posIntersect){
	float t = (esfera.center - ray.getOrigen()).Dot(ray.getDir());
	Vector proj = ray.getOrigen() + ray.getDir() * t;
	Vector dist = (proj - esfera.center);

	posIntersect = SphereIntersect(ray, proj, esfera);

	if (dist.Magnitude() < esfera.radius)
		return true;
	//return Vector(1.0, 0.0, 0.0);
	else
		return false;
		//return Vector(0.0, 0.0, 0.0);
}

Vector SphereIntersect(Ray ray, Vector p, SceneSphere &esfera)
{
	float result1, result2;
	Vector result;

	float radius = esfera.radius * esfera.radius;
	float a = 1.0f;//pow(p.x - esfera.position.x, 2);
	float b = ((ray.getDir() * (ray.getOrigen() - esfera.center)) * 2).Magnitude();//pow(p.y - esfera.position.y, 2);
	float c = pow((ray.getOrigen() - esfera.center).Magnitude(), 2) - radius;//pow(p.z - esfera.position.z, 2);

	float discriminate = b * b - a * c;
	float disc = sqrt(b * b - a * c);

	if (discriminate < 0){
		return Vector(NULL, NULL, NULL);
	}
	else{
		result1 = (-b + disc);
		result2 = (-b - disc);// / 2 * a;

		if (result1 > 0 && result2 > 0)
		{
			if (result1 > result2)
			{
				result = ray.getOrigen() + ray.getDir() * result2;

				return result;
			}
			else
			{
				result = ray.getOrigen() + ray.getDir() * result1;
				return result;
			}
		}
		else if (result1 > 0)
		{
			result = ray.getOrigen() + ray.getDir() * result1;
			//printf("(%f, %f, %f)\n", result.x, result.y, result.z);
			return result;
		}
		else
		{
			result = ray.getOrigen() + ray.getDir() * result2;
			//printf("(%f, %f, %f)\n", result.x, result.y, result.z);
			return result;
		}
	}
}



Vector SphereColor(Scene &escena, SceneSphere sphere, Vector point){
	Vector color = Vector (0.0f, 0.0f, 0.0f);
	Vector mini = Vector(0.0f, 0.0f, 0.0f);
	Vector maxi = Vector(1.0f, 1.0f, 1.0f);

	SceneMaterial *material = escena.GetMaterial(sphere.material);
	
	Vector normal = (sphere.center - point).Normalize();
	Vector V = (escena.GetCamera().GetPosition() - point).Normalize();
	printf("V: (%f, %f, %f)\n", V.x, V.y, V.z);

	for (int i = 0; i < escena.GetNumLights(); i++){
		Vector L = (escena.GetLight(i)->position - point).Normalize();
		//printf("normal: (%f, %f, %f)\n", normal.x, normal.y, normal.z);

		/*(material->diffuse).Magnitude()*/
		Vector diffuse = (escena.GetLight(i)->color) * material->diffuse * normal.Dot(L);
		
		color = color + diffuse.Clamp();
		//printf("COLOR1: (%f, %f, %f)\n", color.x, color.y, color.z);
		//Especular
		////Vector V = point.Normalize() * -1;
		Vector R = normal * 2 * (normal.Dot(L)) - L;
		printf("R: (%f, %f, %f)\n", R.x, R.y, R.z);

		float Sfactor = max(R.Dot(V), 0.0001);
		printf("Sfactor: %f\n", Sfactor);
		Sfactor = pow(Sfactor, material->shininess);
		Vector spec = escena.GetLight(i)->color * material->specular * Sfactor;

		if (spec.x >0 || spec.y >0 || spec.z >0)
			printf("spec: (%f, %f, %f)\n", spec.x, spec.y, spec.z);

		color = color + spec.Clamp();
		//printf("COLOR2: (%f, %f, %f)\n", color.x, color.y, color.z);
		//color += escena.GetLight(i)->; //Probablemente, la constante especular sea 1
	}

	
	return color;
}

/**/
bool TriangleCollision(SceneTriangle &triangle, Ray ray)
{
	//Get Edges
	Vector e1 = triangle.vertex[1] - triangle.vertex[0];
	Vector e2 = triangle.vertex[2] - triangle.vertex[0];

	Vector P, Q, T;
	float det, inv_det, u, v;
	float t;
	float epsilon = 0.0f;

	P = ray.getDir().Cross(e2);
	det = e1.Dot(P);

	if (det > -epsilon && det < epsilon) return false;		
		//return Vector(0.0, 0.0, 1.0);

	inv_det = 1.0f / det;
	
	//First barycentric position coordinate
	T = ray.getOrigen() - triangle.vertex[0];//POSIBLE FALLO
	u = (T.Dot(P)) * inv_det;

	if (u < 0.0f || u > 1.0f)
		return false;
		//return Vector(0.0, 0.0, 1.0);

	Q= T.Cross(e1);
	v = ray.getDir().Dot(Q) * inv_det;

	if (v< 0.0f || (u+v) > 1.0f)
		return false;
		
	return true;

	t = e2.Dot(Q)*inv_det;
	/*
	if (t > epsilon){
		*out = t;
		return 1;

	}*/
	//return Vector(0.0, 1.0, 0.0);
	return 0;
}