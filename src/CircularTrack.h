#pragma once

#include "Track.h"

class CircularTrack : public Track {
    private:
        constexpr static int NUM_SEGMENTS = 250; // number of segments in the circular track
        double r; // radius of the circular track

    public:
        CircularTrack(Eigen::Vector3d pos, float scale, double radius);
        virtual ~CircularTrack() {}
        virtual void draw(std::shared_ptr<MatrixStack> MV, const std::shared_ptr<Program> prog) const override;

        virtual double C(Eigen::Vector3d x) const override;
        virtual double Cn(Eigen::Vector3d x) const override;
        virtual Eigen::Vector3d gradC(Eigen::Vector3d x) const override;
        virtual Eigen::Vector3d gradCn(Eigen::Vector3d x) const override;

        virtual Eigen::Vector3d getForward(Eigen::Vector3d x) const override; // get the forward direction of the track at a given point
};
