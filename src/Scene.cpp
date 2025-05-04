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
	
	auto track1 = make_shared<SplineTrack>(Vector3d(0.0, 0.0, 0.0), 1.0);
	auto track2 = make_shared<SplineTrack>(Vector3d(0.0, 0.0, 0.0), 1.0);
	auto track3 = make_shared<SplineTrack>(Vector3d(0.0, 0.0, 0.0), 1.0);
	auto track4 = make_shared<SplineTrack>(Vector3d(0.0, 0.0, 0.0), 1.0);

	tracks.push_back(track1);
	tracks.push_back(track2);
	tracks.push_back(track3);
	tracks.push_back(track4);

	track = track1;
	slotParticle = make_shared<Particle>(sphereShape);
	car = make_shared<Car>(RESOURCE_DIR, slotParticle);
	ground = make_shared<Ground>();
}

void Scene::init(int track_number)
{
	sphereShape->init();
	ground->init();

	// todo create multiple tracks I could use for demonstration purposes
	// todo we could pass a commandline argument to select the track

	// initialize the tracks
	tracks[0]->addControlPoint(glm::vec3(1.0, 0.0, 0.0));
	tracks[0]->addControlPoint(glm::vec3(0.0, 0.0, 1.0));
	tracks[0]->addControlPoint(glm::vec3(-2.0, 0.0, 0.0));
	tracks[0]->addControlPoint(glm::vec3(-1.0, 1.0, 0.0));
	tracks[0]->addControlPoint(glm::vec3(0.0, 0.0, -1.0));
	tracks[0]->addControlPoint(glm::vec3(1.0, 0.0, 0.0));

	tracks[1]->addControlPoint(glm::vec3(0.0, 2.0, 0.0));
	tracks[1]->addControlPoint(glm::vec3(0.0, 1.0, 1.0));
	tracks[1]->addControlPoint(glm::vec3(-0.5, 0.0, 2.0));
	tracks[1]->addControlPoint(glm::vec3(-1.5, -0.5, 1.0));
	tracks[1]->addControlPoint(glm::vec3(-2.0, 0.0, 0.0));
	tracks[1]->addControlPoint(glm::vec3(-1.0, 1.5, 0.0));
	tracks[1]->addControlPoint(glm::vec3(0.0, 2.0, 0.0));

	tracks[2]->addControlPoint(glm::vec3(0.0, 1.0, -1.0));
	tracks[2]->addControlPoint(glm::vec3(0.5, 0.5, -0.5));
	tracks[2]->addControlPoint(glm::vec3(0.0, 1.0, 0.0));
	tracks[2]->addControlPoint(glm::vec3(0.0, 1.0, 1.0));
	tracks[2]->addControlPoint(glm::vec3(-1.0, 0.5, 2.0));
	tracks[2]->addControlPoint(glm::vec3(1.0, 0.0, -1.0));
	tracks[2]->addControlPoint(glm::vec3(0.5, 0.5, -1.0));
	tracks[2]->addControlPoint(glm::vec3(0.0, 1.0, -1.0));

	tracks[3]->addControlPoint(glm::vec3(1.0, 0.0, 0.0));
	tracks[3]->addControlPoint(glm::vec3(0.0, 0.0, 1.0));
	tracks[3]->addControlPoint(glm::vec3(-2.0, 0.0, 0.0));
	tracks[3]->addControlPoint(glm::vec3(-1.0, 0.0, 0.0));
	tracks[3]->addControlPoint(glm::vec3(0.0, 0.0, -1.0));
	tracks[3]->addControlPoint(glm::vec3(0.0, 0.0, -2.0));
	tracks[3]->addControlPoint(glm::vec3(1.0, 0.0, 0.0));
	
	track = tracks[track_number];

	startingPoints.push_back(Eigen::Vector3d(0.0, 0.0, -1.0));
	startingPoints.push_back(Eigen::Vector3d(0.0, 2.0, 0.0));
	startingPoints.push_back(Eigen::Vector3d(0.0, 1.0, -1.0));
	startingPoints.push_back(Eigen::Vector3d(0.0, 0.0, -1.0));

	// initialize the car
	slotParticle->r = 0.01;
	slotParticle->m = 1.0;
	slotParticle->d = 0.25;
	slotParticle->x0 = startingPoints[track_number];
	slotParticle->x = slotParticle->x0;
	slotParticle->v0 = Vector3d(0.0, 0.0, 0.0);
	slotParticle->v = slotParticle->v0;
	slotParticle->fixed = false;
	slotParticle->p = slotParticle->x0;

	// orient the car to face the direction of the track
	Eigen::Vector3d forward = track->getForward(slotParticle->x);
	car->align_car(glm::vec3(forward(0), forward(1), forward(2)));
	orientation = glm::vec3(forward(0), forward(1), forward(2));
}

void Scene::tare()
{
	slotParticle->tare();
}

void Scene::reset()
{
	t = 0.0;
	slotParticle->reset();

	onGround = 0;
	landed = 0;
	offTrack = 0;

	// orient the car to face the direction of the track
	Eigen::Vector3d forward = track->getForward(slotParticle->x);
	car->align_car(glm::vec3(forward(0), forward(1), forward(2)));
	orientation = glm::vec3(forward(0), forward(1), forward(2));
}

void Scene::step()
{
	t += h;

	if (landed) {
		slotParticle->x(1) = ground->y + slotParticle->r + 0.05f; // set the y coordinate to the ground level
		slotParticle->v(1) = 0.0f; // set the y velocity to 0
		car->align_car(glm::vec3(orientation.x, 0.0f, orientation.z)); // realign the car to face in the direction it was oriented when it fell
		return;
	}

	// update the car's position
	Vector3d f = slotParticle->m * grav - slotParticle->d * slotParticle->v;
	slotParticle->v += (h / slotParticle->m) * f;
	slotParticle->p = slotParticle->x;
	slotParticle->x += h * slotParticle->v;

	// check for ground intersection
	bool nowTouching = groundIntersection();
	bool hitThisFrame = nowTouching && !onGround;

	if (hitThisFrame) { handleGroundIntersection(); }

	onGround = nowTouching;

	// todo might need to add checks for if C or gradC are 0
	// apply constraints to the car particle
	double C = track->C(slotParticle->x);
	Vector3d gradC = track->gradC(slotParticle->x);

	double Cn = track->Cn(slotParticle->x);
	Vector3d gradCn = track->gradCn(slotParticle->x);

	double w = 1.0 / slotParticle->m;
	double lambda = -C / (w * gradC.squaredNorm());
	double lambdaN = -Cn / (w * gradCn.squaredNorm());

	if (offTrack) {
		if (slotParticle->v(1) < 0.0) { car->align_car(glm::vec3(orientation.x, orientation.y, orientation.z)); }
		else { car->align_car(glm::vec3(orientation.x, -orientation.y, orientation.z)); }

		return;
	}

	// if the car goes too fast, don't apply the constraint and let it fall to the ground
	if (slotParticle->v.squaredNorm() * track->getCurvature(slotParticle->x) <= maxCentripital) {
		slotParticle->x += lambda  * w * gradC;
		slotParticle->x += lambdaN * w * gradCn;

	} else {
		offTrack = 1;
		return;
	}

	// update the car's velocity
	slotParticle->v = (1 / h) * (slotParticle->x - slotParticle->p);

	// realign the car to face the direction of the track
	Eigen::Vector3d forward = track->getForward(slotParticle->x);
	car->align_car(glm::vec3(forward(0), forward(1), forward(2)));
	orientation = glm::vec3(forward(0), forward(1), forward(2));
}

void Scene::moveForward()
{
	if (offTrack) { return; }
	slotParticle->v -= track->getForward(slotParticle->x);
}

void Scene::moveBackward()
{
	if (offTrack) { return; }
	slotParticle->v += track->getForward(slotParticle->x);
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

bool Scene::groundIntersection()
{
	return (float) (slotParticle->x(1)) < ground->y;
}

void Scene::handleGroundIntersection()
{
	slotParticle->x(1) = ground->y + slotParticle->r + 0.05f; // set the y coordinate to the ground level);
	
	glm::vec3 v = glm::vec3(slotParticle->v(0), slotParticle->v(1), slotParticle->v(2));

	if (std::abs(v.y) < 1.5f) { v.y = 0.0f; landed = 1; } // stop the car if it's going too slow
	else if (v.y < 0.0) { v.y = -v.y * ground->restitution; } // bounce off the ground

	v.x *= ground->damping; // apply damping
	v.z *= ground->damping; // apply damping

	slotParticle->v = Vector3d(v.x, v.y, v.z);
}
