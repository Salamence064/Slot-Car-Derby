#pragma once

#include <vector>
#include <memory>

#define EIGEN_DONT_ALIGN_STATICALLY
#include <Eigen/Dense>
#include <Eigen/Sparse>

#include "MatrixStack.h"
#include "Program.h"
#include "Shape.h"

class Track {
    protected:
        std::shared_ptr<Shape> shape; // shape of the track // todo find a .obj file for a piece of the track
        std::shared_ptr<Shape> sphere; // represent the particle moving along the track

        Eigen::Vector3d pos; // position of the track
        float scale; // scale of the track

    public:
        inline Track(Eigen::Vector3d pos, float scale) : pos(pos), scale(scale) {
            // // Load the shape of the track from a file
            // shape = std::make_shared<Shape>();
            // shape->loadMesh("path/to/track.obj"); // Replace with actual path to the track mesh file // todo
            // shape->init();
            
            // // Load the sphere shape for the particle
            // sphere = std::make_shared<Shape>();
            // sphere->loadMesh("path/to/sphere.obj"); // Replace with actual path to the sphere mesh file // todo
            // sphere->init();
        }

        virtual ~Track() {}
        virtual void draw(std::shared_ptr<MatrixStack> MV, const std::shared_ptr<Program> prog) const = 0;
        // todo maybe add an init function to load the .obj file to use for the track

        inline Eigen::Vector3d getPos() const { return pos; }
        inline float getScale() const { return scale; }
};
