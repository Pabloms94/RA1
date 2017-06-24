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

   //Calculamos el color con los rebotes del rayo
   return DrawRay(ray, 2, la_escena, -1);
}

Vector RayTrace::DrawRay(Ray ray, int rebotes, Scene &la_escena, int idCollision)
{
	if (rebotes < 1)
		return Vector(0.0f, 0.0f, 0.0f);
	else
	{
		float targetDistance = INFINITY;
		Vector color(0.0f, 0.0f, 0.0f);
		Vector interPoint, normal;
		bool dontIntersect = true;
		Ray reflectRay;
		int id = -1;

		for (int i = 0; i < la_escena.GetNumObjects(); i++)
		{
			if (la_escena.GetObject(i)->IsSphere())
			{
				if (SphereCollision(*(SceneSphere*)la_escena.GetObject(i), ray, interPoint) && idCollision != i)
				{
					dontIntersect = false;
					float distCalc = (interPoint - ray.getOrigen()).Magnitude();
					if (targetDistance > distCalc)
					{
						targetDistance = distCalc;
						normal = (interPoint - (*(SceneSphere*)la_escena.GetObject(i)).center).Normalize();
						color = SphereColor(la_escena, *(SceneSphere*)la_escena.GetObject(i), interPoint);
						id = i;

						reflectRay = Ray(interPoint, Reflection(ray.getDir(), normal));
					}
				}
			}
			
			if (la_escena.GetObject(i)->IsTriangle())
			{
				if (TriangleCollision(*(SceneTriangle*)la_escena.GetObject(i), ray, interPoint)/* && idCollision != i*/)
				{
					dontIntersect = false;
					float distCalc = (interPoint - ray.getOrigen()).Magnitude();
					if (targetDistance > distCalc)
					{
						targetDistance = distCalc;
						SceneTriangle triangle = *(SceneTriangle*)la_escena.GetObject(i);
						Vector bary = Barycentric(ray.getOrigen(), triangle.vertex[0], triangle.vertex[1], triangle.vertex[2]);
						color = TriangleColor(la_escena, triangle, interPoint, bary.x, bary.y, bary.z);
						normal = (triangle.normal[0] * bary.x + triangle.normal[1] * bary.y + triangle.normal[2] * bary.z).Normalize();
						reflectRay = Ray(interPoint, Reflection(ray.getDir(), normal));
						id = i;
					}
				}
			}
			
			if (la_escena.GetObject(i)->IsModel() && idCollision != i)
			{
				int nTriangles = (*(SceneModel*)la_escena.GetObject(i)).GetNumTriangles();
				for (int j = 0; j < nTriangles; j++){
					SceneTriangle triangle = *(*(SceneModel*)la_escena.GetObject(i)).GetTriangle(j);

					if (TriangleCollision(triangle, ray, interPoint)){
						dontIntersect = false;
						float distCalc = (interPoint - ray.getOrigen()).Magnitude();
						if (targetDistance > distCalc)
						{
							targetDistance = distCalc;
							Vector bary = Barycentric(ray.getDir(), triangle.vertex[0], triangle.vertex[1], triangle.vertex[2]);
							color = TriangleColor(la_escena, triangle, interPoint, bary.x, bary.y, bary.z);
							normal = (triangle.normal[0] * bary.x + triangle.normal[1] * bary.y + triangle.normal[2] * bary.z).Normalize();
							reflectRay = Ray(interPoint, Reflection(ray.getDir(), normal));
							id = i;
						}
					}
				}
			}
		}

		if (dontIntersect && rebotes == 1){
			return Vector(0.0f, 0.0f, 0.0f);
		}
		else if (dontIntersect)
			return la_escena.GetBackground().color;

		
		return (color + DrawRay(reflectRay, rebotes - 1, la_escena, id));
	}
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

bool RayTrace::SphereCollision(SceneSphere &esfera, Ray ray, Vector &posIntersect){
	float t = (esfera.center - ray.getOrigen()).Dot(ray.getDir());
	Vector proj = ray.getOrigen() + ray.getDir() * t;
	float dist = (proj - esfera.center).Magnitude();

	posIntersect = SphereIntersect(ray, esfera);

	if (dist < esfera.radius)
		return true;
	
	return false;
}

Vector RayTrace::SphereIntersect(Ray ray, SceneSphere &esfera)
{
	float result1, result2;
	Vector result;

	float radius2 = esfera.radius * esfera.radius;
	float a = ray.getDir().Dot(ray.getDir());
	float b = (ray.getDir().Dot(ray.getOrigen() - esfera.center)) * 2;
	float c = (ray.getOrigen() - esfera.center).Dot(ray.getOrigen() - esfera.center) - radius2;

	float discriminate = b * b - 4 * a * c;
	float disc = sqrt(b * b - 4 * a * c);

	if (discriminate < 0){
		return Vector(NULL, NULL, NULL);
	}
	else{
		result1 = (-b + disc) / (2 * a);
		result2 = (-b - disc) / (2 * a);

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
			return result;
		}
		else
		{
			result = ray.getOrigen() + ray.getDir() * result2;
			return result;
		}
	}
}

Vector RayTrace::SphereColor(Scene &escena, SceneSphere sphere, Vector point){
	Vector color = Vector (0.0f, 0.0f, 0.0f);
	color = color + (Vector)(escena.GetBackground().ambientLight) * 0.07f;

	SceneMaterial *material = escena.GetMaterial(sphere.material);

	Vector normal = (point - sphere.center).Normalize();
	Vector V = (escena.GetCamera().GetPosition() - point).Normalize();

	for (int i = 0; i < escena.GetNumLights(); i++){
		//Difusa
		if (IsCastShadow(point, escena.GetLight(i)->position, escena))
		{
			color = (Vector)(escena.GetBackground().ambientLight) * 0.07f;
		}
		else
		{
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
	}

	return color;
}

Vector RayTrace::TriangleColor(Scene &escena, SceneTriangle &triangle, Vector &point, float &u, float &v, float &w){
	Vector color = Vector(0.0f, 0.0f, 0.0f);
	color = color + (Vector)(escena.GetBackground().ambientLight) * 0.07f;	//0.07f coeficiente ambiental....

	SceneMaterial *material0 = escena.GetMaterial(triangle.material[0]);
	SceneMaterial *material1 = escena.GetMaterial(triangle.material[1]);
	SceneMaterial *material2 = escena.GetMaterial(triangle.material[2]);

	Vector diffuse = material0->diffuse * u + material1->diffuse * v + material2->diffuse * w;
	Vector specular = material0->specular * u + material1->specular * v + material2->specular * w;
	float sh = material0->shininess * u + material1->shininess * v + material2->shininess * w;

	Vector N = (triangle.normal[0] * u + triangle.normal[1] * v + triangle.normal[2] * w).Normalize();
	Vector V = (escena.GetCamera().GetPosition() - point).Normalize();

	for (int i = 0; i < escena.GetNumLights(); i++){
		if (IsCastShadow(point, escena.GetLight(i)->position, escena))
		{
			color = (Vector)(escena.GetBackground().ambientLight) * 0.07f;
		}
		else
		{
			//Difusa
			Vector L = (escena.GetLight(i)->position - point).Normalize();
			Vector diff = escena.GetLight(i)->color * diffuse * N.Dot(L);

			//printf("u: %f, v: %f, w: %f\n", u, v, w);
			//printf("diffuse  u: %f, v: %f, w: %f\n", diffuse.x, diffuse.y, diffuse.z);

			color = color + diff.Clamp();
			//printf("diff  r: %f, g: %f, b: %f\n", color.x, color.y, color.z);

			Vector R = (N * 2 * N.Dot(L) - L).Normalize();
			float Sfactor = R.Dot(V);

			Sfactor = pow(Sfactor, sh);
			Vector spec = escena.GetLight(i)->color * specular * Sfactor;
			color = color + spec.Clamp();
			//printf("spec  r: %f, g: %f, b: %f\n", color.x, color.y, color.z);
		}
	}

	return color;
}

bool RayTrace::TriangleCollision(SceneTriangle &triangle, Ray ray, Vector &intPoint)
{
	float u, v;
	//Get Edges
	Vector e1 = triangle.vertex[1] - triangle.vertex[0];
	Vector e2 = triangle.vertex[2] - triangle.vertex[0];

	Vector P, Q, T;
	float det, inv_det;
	float t;
	float epsilon = 0.0f;

	P = ray.getDir().Cross(e2);
	det = e1.Dot(P);

	if (det > -epsilon && det < epsilon) return false;		

	inv_det = 1.0f / det;
	
	//First barycentric position coordinate
	T = ray.getOrigen() - triangle.vertex[0];
	u = (T.Dot(P)) * inv_det;

	if (u < 0.0f || u > 1.0f)
		return false;

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

bool RayTrace::IsCastShadow(Vector orig, Vector pLight, Scene &la_escena)
{
	Ray ray;
	ray.setOrigen(orig);
	ray.setDir(pLight - orig);
	Vector posI;

	bool oclusion = false;

	for (int i = 0; i < la_escena.GetNumObjects() && !oclusion; i++)
	{
		if (la_escena.GetObject(i)->IsSphere())
		{
			if (SphereCollision(*(SceneSphere*)la_escena.GetObject(i), ray, posI)){
				if (GetDistance(orig, pLight) > GetDistance(posI, pLight))
					oclusion = true;
			}
		}
		else if (la_escena.GetObject(i)->IsTriangle())
		{
			if (TriangleCollision(*(SceneTriangle*)la_escena.GetObject(i), ray, posI))
			{
				if (GetDistance(orig, pLight) > GetDistance(posI, pLight))
					oclusion = true;
			}
		}
		else if (la_escena.GetObject(i)->IsModel())
		{
			unsigned int nTriangles = (*(SceneModel*)la_escena.GetObject(i)).GetNumTriangles();
			for (int j = 0; j < nTriangles && !oclusion; j++){
				if (TriangleCollision(*(*(SceneModel*)la_escena.GetObject(i)).GetTriangle(j), ray, posI)){
					if (GetDistance(orig, pLight) > GetDistance(posI, pLight))
						oclusion = true;
				}
			}
		}
	}
	return oclusion;
}

float RayTrace::GetDistance(Vector origen, Vector destino){
	return (sqrt(pow(destino.x - origen.x, 2) + (pow(destino.y - origen.y, 2)) + (pow(destino.z - origen.z, 2))));
}

Vector RayTrace::Barycentric(Vector p, Vector a, Vector b, Vector c)		//CAMBIAR
{
	Vector v0 = b - a, v1 = c - a, v2 = p - a;
	float d00 = v0.Dot(v0);
	float d01 = v0.Dot(v1);
	float d11 = v1.Dot(v1);
	float d20 = v2.Dot(v0);
	float d21 = v2.Dot(v1);
	float denom = d00 * d11 - d01 * d01;
	float v = (d11 * d20 - d01 * d21) / denom;
	float w = (d00 * d21 - d01 * d20) / denom;
	float u = 1.0f - v - w;

	return Vector(u,v,w);
}

Vector RayTrace::Reflection(Vector dir, Vector normal)
{
	return dir - normal * (dir.Dot(normal)) * 2;
}

Vector RayTrace::CalculatePixelAntialiasing(int screenX, int screenY)
{
	Scene &la_escena = m_Scene;
	Camera mycam = la_escena.GetCamera();
	Vector posicion = mycam.GetPosition();

	if ((screenX < 0 || screenX > Scene::WINDOW_WIDTH - 1) ||
		(screenY < 0 || screenY > Scene::WINDOW_HEIGHT - 1))
	{
		// Off the screen, return black
		return Vector(0.0f, 0.0f, 0.0f);
	}

	//Configuramos la cámara
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

	//Calculamos los rayos, al hacer un antialiasing x4, tenemos que tirar 4 rayos por pixel
	//y calcular el valor medio de estos.
	Ray *ray = Calculate4Rays(screenX, screenY);

	Vector colorFinal;

	//Calculamos el color con los rebotes del rayo de cada uno de los cuatro rayos
	for (int i = 0; i < 4; i++)
		colorFinal = colorFinal + DrawRay(ray[i], 2, la_escena, -1) * 0.25f;

	return colorFinal;
}

Ray	* RayTrace::Calculate4Rays(int screenX, int screenY)
{
	// Para calcular los cuatros rayos voy a dividir el pixel en 4 regiones y
	// para evitar posibles artefactos los genero su dirección en sus regiones
	// de forma aleatoria.

	Ray *fourRays = new Ray[4];

	int cont = 0;

	for (float x_ant = 0, x = 0.5f; x <= 1.0f; x_ant += 0.5f, x += 0.5)
	{
		for (float y_ant = 0, y = 0.5f; y <= 1.0f; y_ant += 0.5f, y += 0.5, cont++)
		{
			float diff = x - x_ant;
			float r = (((float)rand()) / (float)RAND_MAX) * diff;
			float coordX = x_ant + r;

			diff = y - y_ant;
			r = (((float)rand()) / (float)RAND_MAX) * diff;
			float coordY = y_ant + r;

			float cX = float(screenX + coordX);
			float cY = float(screenY + coordY);

			float pixelCameraX = (cX * 2.0f / (float)m_Scene.WINDOW_WIDTH) - 1.0f;
			float pixelCameraY = (cY * 2.0f / (float)m_Scene.WINDOW_HEIGHT) - 1.0f;

			Vector dir;
			up = right.Cross(lookat);
			dir = lookat + right * aspect * tangen * pixelCameraX + up * tangen * pixelCameraY;
			dir.Normalize();

			fourRays[cont].setDir(dir);
			fourRays[cont].setOrigen(pos);
		}
	}

	return fourRays;
}