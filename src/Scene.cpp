#include <iostream>

#include "SplineTrack.h"
#include "CircularTrack.h"
#include "GLSL.h"
#include "Scene.h"
#include "Particle.h"
#include "Shape.h"
#include "Program.h"

using namespace std;
using namespace Eigen;

Scene::Scene() :
	t(0.0),
	h(1e-3),
	grav(0.0, 0.0, 0.0)
{
}

Scene::~Scene()
{
}

void Scene::load(const string &RESOURCE_DIR)
{
	// Units: meters, kilograms, seconds
	h = 1e-3; // * time step
	
	grav << 0.0, -9.8, 0.0;
	
	sphereShape = make_shared<Shape>();
	sphereShape->loadMesh(RESOURCE_DIR + "sphere2.obj");
	
	track = make_shared<SplineTrack>(Vector3d(0.0, 0.0, 0.0), 1.0);

	car = make_shared<Particle>(sphereShape);
}

void Scene::init()
{
	sphereShape->init();

	// ! Remember to duplicate the starting and ending control points
	// initialize the track by adding control points
	track->addControlPoint(glm::vec3(1.0, 0.0, 0.0));
	track->addControlPoint(glm::vec3(1.0, 0.0, 0.0));
	track->addControlPoint(glm::vec3(0.0, 0.0, 1.0));
	track->addControlPoint(glm::vec3(-2.0, 0.0, 0.0));
	track->addControlPoint(glm::vec3(-1.0, 1.0, 0.0));
	track->addControlPoint(glm::vec3(0.0, 0.0, -1.0));
	track->addControlPoint(glm::vec3(1.0, 0.0, 0.0));
	track->addControlPoint(glm::vec3(1.0, 0.0, 0.0));
	
	// initialize the car
	car->r = 0.01;
	car->m = 1.0;
	car->d = 0.25;
	car->x0 = Vector3d(0.0, 0.0, -1.0);
	car->x = car->x0;
	car->v0 = Vector3d(0.0, 0.0, 0.0);
	car->v = car->v0;
	car->fixed = false;
	car->p = car->x0;
}

void Scene::tare()
{
	car->tare();
}

void Scene::reset()
{
	t = 0.0;
	car->reset();
}

void Scene::step()
{
	t += h;

	// todo add gravity in the future when we are working with moving up and down (along the y-axis)
	// update the car's position
	Vector3d f = car->d * car->v;
	car->v += (h / car->m) * f;
	car->p = car->x;
	car->x += h * car->v;

	// todo might need to add checks for if C or gradC are 0
	// apply constraints to the car particle
	double C = track->C(car->x);
	Vector3d gradC = track->gradC(car->x);

	double Cn = track->Cn(car->x);
	Vector3d gradCn = track->gradCn(car->x);

	double w = 1.0 / car->m;
	double lambda = -C / (w * gradC.squaredNorm());
	double lambdaN = -Cn / (w * gradCn.squaredNorm());

	car->x += lambda  * w * gradC;
	car->x += lambdaN * w * gradCn;

	// update the car's velocity
	car->v = (1 / h) * (car->x - car->p);
}

void Scene::moveClockwise()
{ // todo we'll need to find better values for the speed
	car->v += 0.1 * track->getForward(car->x);
}

void Scene::moveCounterClockwise()
{ // todo we'll need to find better values for the speed
	car->v += 0.1 * -track->getForward(car->x);
}

void Scene::draw(shared_ptr<MatrixStack> MV, const shared_ptr<Program> prog, const shared_ptr<Program> progSimple) const
{
	glUniform3fv(prog->getUniform("kdFront"), 1, Vector3f(1.0, 1.0, 1.0).data());
	car->draw(MV, prog);
	prog->unbind();

	progSimple->bind();
	track->draw(MV, progSimple);
	progSimple->unbind();
}
