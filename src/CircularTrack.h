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
};
