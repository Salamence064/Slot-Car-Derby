#include <iostream>

#include "SplineTrack.h"
#include "CircularTrack.h"
#include "GLSL.h"
#include "Scene.h"
#include "Particle.h"
#include "Shape.h"
#include "Program.h"
#include "Car.h"
#include "ground.h"

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
	sphereShape->computeMinMax();
	
	track = make_shared<SplineTrack>(Vector3d(0.0, 0.0, 0.0), 1.0);
	slotParticle = make_shared<Particle>(sphereShape);
	car = make_shared<Car>(RESOURCE_DIR, slotParticle);
	ground = make_shared<Ground>();
}

void Scene::init()
{
	sphereShape->init();
	ground->init();

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
	slotParticle->r = 0.01;
	slotParticle->m = 1.0;
	slotParticle->d = 0.25;
	slotParticle->x0 = Vector3d(0.0, 0.0, -1.0);
	slotParticle->x = slotParticle->x0;
	slotParticle->v0 = Vector3d(0.0, 0.0, 0.0);
	slotParticle->v = slotParticle->v0;
	slotParticle->fixed = false;
	slotParticle->p = slotParticle->x0;
}

void Scene::tare()
{
	slotParticle->tare();
}

void Scene::reset()
{
	t = 0.0;
	slotParticle->reset();
}

void Scene::step()
{
	t += h;

	// todo add gravity in the future when we are working with moving up and down (along the y-axis)
	// update the car's position
	Vector3d f = slotParticle->d * slotParticle->v;
	slotParticle->v += (h / slotParticle->m) * f;
	slotParticle->p = slotParticle->x;
	slotParticle->x += h * slotParticle->v;

	// todo might need to add checks for if C or gradC are 0
	// apply constraints to the car particle
	double C = track->C(slotParticle->x);
	Vector3d gradC = track->gradC(slotParticle->x);

	double Cn = track->Cn(slotParticle->x);
	Vector3d gradCn = track->gradCn(slotParticle->x);

	double w = 1.0 / slotParticle->m;
	double lambda = -C / (w * gradC.squaredNorm());
	double lambdaN = -Cn / (w * gradCn.squaredNorm());

	slotParticle->x += lambda  * w * gradC;
	slotParticle->x += lambdaN * w * gradCn;

	// update the car's velocity
	slotParticle->v = (1 / h) * (slotParticle->x - slotParticle->p);

	Eigen::Vector3d forward = track->getForward(slotParticle->x);
	car->align_car(glm::vec3(forward(0), forward(1), forward(2))); // todo the tangent vector isn't really the correct "forward" direction
}

void Scene::moveClockwise()
{ // todo we'll need to find better values for the speed
	slotParticle->v += 0.1 * track->getForward(slotParticle->x);
}

void Scene::moveCounterClockwise()
{ // todo we'll need to find better values for the speed
	slotParticle->v += 0.1 * -track->getForward(slotParticle->x);
}

void Scene::draw(shared_ptr<MatrixStack> MV, const shared_ptr<Program> prog, const shared_ptr<Program> progSimple) const
{
	glUniform3fv(prog->getUniform("kdFront"), 1, Vector3f(1.0, 1.0, 1.0).data());
	ground->draw(prog, MV);
	car->draw(MV, prog);
	prog->unbind();

	progSimple->bind();
	track->draw(MV, progSimple);
	progSimple->unbind();
}
