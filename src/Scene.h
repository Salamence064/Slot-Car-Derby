#pragma once
#ifndef Scene_H
#define Scene_H

#include <vector>
#include <memory>
#include <string>

#define EIGEN_DONT_ALIGN_STATICALLY
#include <Eigen/Dense>

class Track;
class Particle;
class MatrixStack;
class Program;
class Shape;
class Car;
class Ground;

class Scene
{
public:
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW
	
	Scene();
	virtual ~Scene();
	
	void load(const std::string &RESOURCE_DIR);
	void init();
	void tare();
	void reset();
	void step();

	void moveForward();
	void moveBackward();
	
	void draw(std::shared_ptr<MatrixStack> MV, const std::shared_ptr<Program> prog, const std::shared_ptr<Program> progSimple) const;
	
	double getTime() const { return t; }
	
private:
	static constexpr double maxCentripital = 10000.0; // maximum centripital force

	double t;
	double h;
	Eigen::Vector3d grav;
	
	std::shared_ptr<Shape> sphereShape;
	std::shared_ptr<Track> track;
	std::shared_ptr<Car> car;

	std::shared_ptr<Particle> slotParticle;
	std::shared_ptr<Ground> ground;
};

#endif
