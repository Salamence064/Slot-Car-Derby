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
    public:
        std::shared_ptr<Shape> shape; // shape of the track // todo find a .obj file for a piece of the track

        Eigen::Vector3d pos; // position of the track
        float scale; // scale of the track

    public:
        Track() {}
        virtual ~Track() {}

        virtual void init(Eigen::Vector3d pos, float scale) {
            this->pos = pos;
            this->scale = scale;

            // // Load the shape of the track from a file
            // shape = std::make_shared<Shape>();
            // shape->loadMesh("path/to/track.obj"); // Replace with actual path to the track mesh file // todo
            // shape->init();
        }

        virtual void draw(std::shared_ptr<MatrixStack> MV, const std::shared_ptr<Program> prog) const = 0;
        virtual double C(Eigen::Vector3d x) const = 0; // constraint function for the track
        virtual Eigen::Vector3d gradC(Eigen::Vector3d x) const = 0; // gradient of the constraint function

        inline Eigen::Vector3d getPos() const { return pos; }
        inline float getScale() const { return scale; }
};
