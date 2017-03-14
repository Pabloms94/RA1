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
   
   SceneSphere sphere;
   SceneTriangle triangle;
   SceneModel model;
   Vector posI;

   bool isIntersect = false;
   for (int i = 0; i < la_escena.GetNumObjects(); i++)
   {
		if (la_escena.GetObject(i)->IsSphere())
		{
			if (SphereCollision(*(SceneSphere*)la_escena.GetObject(i), ray, posI)){
				if (IsCastShadow(posI, la_escena.GetLight(0)->position, la_escena))
					return Vector(0, 0, 0);
				sphere = (*(SceneSphere*)la_escena.GetObject(i));
				return SphereColor(la_escena, sphere, posI);
			}
		}
		else if (la_escena.GetObject(i)->IsTriangle())
		{
			float u, v, t;
			bool dentro = false;
			if (rayTriangleIntersect(*(SceneTriangle*)la_escena.GetObject(i), ray, posI, t, u, v)){
			//if (TriangleCollision(*(SceneTriangle*)la_escena.GetObject(i), ray, posI, u, v)){
				if (IsCastShadow(posI, la_escena.GetLight(0)->position, la_escena))
					return Vector(0, 0, 0);
				triangle = *(SceneTriangle*)la_escena.GetObject(i);
				return TriangleColor(la_escena, triangle, posI, u, v, dentro);
			}
		}
		else if (la_escena.GetObject(i)->IsModel())
		{
			unsigned int nTriangles = (*(SceneModel*)la_escena.GetObject(i)).GetNumTriangles();
			bool dentro = true;
			for (int j = 0; j < nTriangles; j++){
				float u, v, t;
				//if (TriangleCollision(*(*(SceneModel*)la_escena.GetObject(i)).GetTriangle(j), ray, posI, u, v)) {
				
				if (rayTriangleIntersect(*(*(SceneModel*)la_escena.GetObject(i)).GetTriangle(j), ray, posI, t, u, v)){
					if (IsCastShadow(posI, la_escena.GetLight(0)->position, la_escena))
						return Vector(0, 0, 0);
					triangle = *(*(SceneModel*)la_escena.GetObject(i)).GetTriangle(j);
					return TriangleColor(la_escena, triangle, posI, u, v, dentro);
				}
					
			}
		}
	}
	
	return la_escena.GetBackground().color;
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

	float radius2 = esfera.radius * esfera.radius;
	float a = ray.getDir().Dot(ray.getDir());//pow(p.x - esfera.position.x, 2);
	float b = (ray.getDir().Dot(ray.getOrigen() - esfera.center)) * 2;//pow(p.y - esfera.position.y, 2);
	float c = (ray.getOrigen() - esfera.center).Dot(ray.getOrigen() - esfera.center) - radius2;//pow(p.z - esfera.position.z, 2);

	float discriminate = b * b - 4 * a * c;
	float disc = sqrt(b * b - 4 * a * c);

	if (discriminate < 0){
		return Vector(NULL, NULL, NULL);
	}
	else{
		result1 = (-b + disc) / (2 * a);
		result2 = (-b - disc) / (2 * a);// / 2 * a;

		if (result1 > 0 && result2 > 0)
		{
			if (result1 > result2)
			{
				result = ray.getOrigen() + ray.getDir() * result2;
				//printf("(%f, %f, %f)\n", result.x, result.y, result.z);

				return result;
			}
			else
			{
				result = ray.getOrigen() + ray.getDir() * result1;
				//printf("(%f, %f, %f)\n", result.x, result.y, result.z);

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

	color = color + (Vector)(escena.GetBackground().ambientLight) * 0.07f;

	SceneMaterial *material = escena.GetMaterial(sphere.material);
	
	//Vector normal = (sphere.center - point).Normalize();
	Vector normal = (point - sphere.center).Normalize();
	Vector V = (escena.GetCamera().GetPosition() - point).Normalize();

	for (int i = 0; i < escena.GetNumLights(); i++){
		//Difusa
		Vector L = (escena.GetLight(i)->position - point).Normalize();
		Vector diffuse = (escena.GetLight(i)->color) * material->diffuse * normal.Dot(L);
		
		color = color + diffuse.Clamp();

		//Especular
		Vector R = normal * 2 * (normal.Dot(L)) - L;
		Vector V = (escena.GetCamera().GetPosition() - point).Normalize();
		float Sfactor = R.Dot(V);

		Sfactor = pow(Sfactor, material->shininess);
		Vector spec = escena.GetLight(i)->color * material->specular * Sfactor;

		color = color + spec.Clamp();
	}

	return color;
}

Vector TriangleColor(Scene &escena, SceneTriangle &triangle, Vector &point, float &u, float &v, bool &dentro){
	Vector color = Vector(0.0f, 0.0f, 0.0f);
	color = color + (Vector)(escena.GetBackground().ambientLight) * 0.07f;

	Vector normal = triangle.normal[0] * u + triangle.normal[1] * v + triangle.normal[2] * (1 - u - v);
	normal = normal / normal.Magnitude();
	//Vector normal = (triangle.vertex[1] - triangle.vertex[0]).Cross(triangle.vertex[2] - triangle.vertex[0]);
	//normal = normal / normal.Magnitude();

	//printf("u: %f, v: %f, w: %f\n",u, v, (1 - u - v));
	//Vector normal = triangle.normal[0] + triangle.normal[1] + triangle.normal[2] ;

	Vector diffuse = (escena.GetMaterial(triangle.material[1])->diffuse) * u + (escena.GetMaterial(triangle.material[2])->diffuse) * v + (escena.GetMaterial(triangle.material[0])->diffuse) * (1 - u - v);
	//diffuse = diffuse / diffuse.Magnitude();
	//Vector especular = (escena.GetMaterial(triangle.material[1])->specular) * u + (escena.GetMaterial(triangle.material[2])->specular) * v + (escena.GetMaterial(triangle.material[0])->specular) * (1 - u - v);
	//especular = especular / especular.Magnitude();
	//float shininess = (escena.GetMaterial(triangle.material[1])->shininess) * u + (escena.GetMaterial(triangle.material[2])->shininess) * v + (escena.GetMaterial(triangle.material[0])->shininess) * (1 - u - v);

	//for (int i = 0; i < escena.GetNumLights(); i++){
		int i = 0;
		Vector L = (escena.GetLight(i)->position - point).Normalize();
		Vector diff = escena.GetLight(i)->color * diffuse * normal.Dot(L);
		//if (normal.Dot(L) < 1.f){
		if (dentro){
			printf("normal = %f, %f, %f\n", normal.x, normal.y, normal.z);
			printf("L = %f, %f, %f\n", L.x, L.y, L.z);
			printf("normal dot L = %f\n", normal.Dot(L));
		}
		//}
		color = color + diff;// .Clamp();

		//Especular
		//Vector R = normal * 2.0f * (normal.Dot(L)) - L;
		//Vector V = (escena.GetCamera().GetPosition() - point).Normalize();
		//float Sfactor = R.Dot(V);

		//Sfactor = pow(Sfactor, shininess);
		//Vector spec = escena.GetLight(i)->color * especular * Sfactor;

		//color = color + spec.Clamp();
	//}
		dentro = false;

	return color;
}

/**/
bool TriangleCollision(SceneTriangle &triangle, Ray ray, Vector &intPoint, float &u, float &v)
{
	//Get Edges
	Vector e1 = triangle.vertex[1] - triangle.vertex[0];
	Vector e2 = triangle.vertex[2] - triangle.vertex[0];

	Vector P, Q, T;
	float det, inv_det;
	float t;
	float epsilon = 0.0f;

	P = ray.getDir().Cross(e2);
	det = e1.Dot(P);

	if (/*det > -epsilon &&*/ det < epsilon) return false;		
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
		

	t = e2.Dot(Q)*inv_det;

	intPoint = ray.getOrigen() + ray.getDir() * t;

	return true;

	/*
	if (t > epsilon){
		*out = t;
		return 1;

	}*/
	//return Vector(0.0, 1.0, 0.0);
}

bool rayTriangleIntersect(SceneTriangle &triangle, Ray ray, Vector &intPoint, float &t, float &u, float &v)
{
	float const EPSILON = 0.000001;
	Vector edge1 = triangle.vertex[1] - triangle.vertex[0];
	Vector edge2 = triangle.vertex[2] - triangle.vertex[0];
	//Calculamos el determinante
	Vector pvec = ray.getDir().Cross(edge2);
	float det = edge1.Dot(pvec);

	if (det < EPSILON) return false;

	//Calculamos distancia del vertice 0 al origen del rayo
	Vector tvec = ray.getOrigen() - triangle.vertex[0];

	//Calculamos el parámetro U y lo testeamos
	u = tvec.Dot(pvec);// *invDet;
	if (u < 0.0 || u > det) return false;

	//Calculamos V y lo testeamos
	Vector qvec = tvec.Cross(edge1);
	v = ray.getDir().Dot(qvec);// *invDet;
	if (v < 0.0 || (u + v) > det) return false;

	//Calculamos t
	t = edge2.Dot(qvec);// *invDet;

	float invDet = 1.0 / det;

	t *= invDet;
	u *= invDet;
	v *= invDet;

	return true;
}

bool IsCastShadow(Vector orig, Vector pLight, Scene &la_escena)
{
	Ray ray;
	ray.setOrigen(orig);
	ray.setDir(pLight - orig);
	Vector posI;

	for (int i = 0; i < la_escena.GetNumObjects(); i++)
	{
		if (la_escena.GetObject(i)->IsSphere())
		{
			if (SphereCollision(*(SceneSphere*)la_escena.GetObject(i), ray, posI)){
				//if ((ray.getDir().Magnitude() >(orig - posI).Magnitude()) && (ray.getDir().Magnitude() > (posI - pLight).Magnitude()))
					return true;
			}
		}
		else if (la_escena.GetObject(i)->IsTriangle())
		{
			float u, v, t;
			bool dentro = false;
			if (rayTriangleIntersect(*(SceneTriangle*)la_escena.GetObject(i), ray, posI, t, u, v)){
				//if (ray.getDir().Magnitude() > (orig - posI).Magnitude() && (ray.getDir().Magnitude() > (posI - pLight).Magnitude()))
					return true;
			}
		}
		else if (la_escena.GetObject(i)->IsModel())
		{
			unsigned int nTriangles = (*(SceneModel*)la_escena.GetObject(i)).GetNumTriangles();
			bool dentro = true;
			for (int j = 0; j < nTriangles; j++){
				float u, v, t;
				//if (TriangleCollision(*(*(SceneModel*)la_escena.GetObject(i)).GetTriangle(j), ray, posI, u, v)) {

				if (rayTriangleIntersect(*(*(SceneModel*)la_escena.GetObject(i)).GetTriangle(j), ray, posI, t, u, v)){
					//if (ray.getDir().Magnitude() >(orig - posI).Magnitude() && (ray.getDir().Magnitude() > (posI - pLight).Magnitude()))
						return true;
				}
			}
		}
	}
	return false;
}