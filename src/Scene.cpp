#include <iostream>

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
	h(1e-2),
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
	
	track = make_shared<CircularTrack>(Vector3d(0.0, 0.0, 0.0), 1.0, 1.0);

	auto sphere = make_shared<Particle>(sphereShape);
	spheres.push_back(sphere);
	sphere->r = 0.1;
	sphere->x = Vector3d(0.0, 0.2, 0.0);
}

void Scene::init()
{
	sphereShape->init();
	// todo maybe init the track
	// todo init the car
}

void Scene::tare()
{
	for(auto s : spheres) {
		s->tare();
	}

	// todo tare the car
}

void Scene::reset()
{
	t = 0.0;
	for(auto s : spheres) {
		s->reset();
	}

	// todo reset the car
}

void Scene::step()
{
	t += h;
	
	// Move the sphere
	if(!spheres.empty()) {
		auto s = spheres.front();
		s->x(2) = 0.5 * sin(0.5*t);
	}

	// todo simulate the car
}

void Scene::draw(shared_ptr<MatrixStack> MV, const shared_ptr<Program> prog, const shared_ptr<Program> progSimple) const
{
	glUniform3fv(prog->getUniform("kdFront"), 1, Vector3f(1.0, 1.0, 1.0).data());
	for(auto s : spheres) {
		s->draw(MV, prog);
	}

	prog->unbind();

	progSimple->bind();
	track->draw(MV, progSimple);
	progSimple->unbind();

	// todo draw the car
}
