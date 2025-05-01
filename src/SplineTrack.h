#pragma once

#include <glm/glm.hpp>

#include "Track.h"

class SplineTrack : public Track
{
    private:
        const glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f); // up direction (normal to the track)

        std::vector<glm::vec3> cps; // control points
        glm::mat4 B; // basis matrix for catmull-rom spline

    public:
        SplineTrack(Eigen::Vector3d pos, float scale);
        ~SplineTrack() {}

        virtual void addControlPoint(const glm::vec3& cp) override { cps.push_back(cp); } // add a control point

        virtual void draw(std::shared_ptr<MatrixStack> MV, const std::shared_ptr<Program> prog) const override;

        virtual double C(Eigen::Vector3d x) const override; // constraint function for the track
        virtual double Cn(Eigen::Vector3d x) const override; // constraint function for the track (normal)
        virtual Eigen::Vector3d gradC(Eigen::Vector3d x) const override; // gradient of the constraint function
        virtual Eigen::Vector3d gradCn(Eigen::Vector3d x) const override; // gradient of the constraint function (normal)

        virtual Eigen::Vector3d getForward(Eigen::Vector3d x) const override; // get the forward direction of the track at a given point
};
