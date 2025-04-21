#pragma once

#include <glm/glm.hpp>

#include "Track.h"

class SplineTrack : public Track
{
    private:
        std::vector<glm::vec3> cps; // control points
        glm::mat4 B; // basis matrix for catmull-rom spline

    public:
        SplineTrack(Eigen::Vector3d pos, float scale);
        ~SplineTrack() {}

        virtual void addControlPoint(const glm::vec3& cp) override { cps.push_back(cp); } // add a control point

        virtual void draw(std::shared_ptr<MatrixStack> MV, const std::shared_ptr<Program> prog) const override;

        virtual double C(Eigen::Vector3d x) const override; // constraint function for the track
        virtual Eigen::Vector3d gradC(Eigen::Vector3d x) const override; // gradient of the constraint function
};
