#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif
#include <GL/glew.h>

#include "CircularTrack.h"

CircularTrack::CircularTrack(Eigen::Vector3d pos, float scale, double radius) : Track(pos, scale), r(radius) {}

void CircularTrack::draw(std::shared_ptr<MatrixStack> MV, const std::shared_ptr<Program> prog) const {
    glBegin(GL_LINE);

    for (int i = 0; i < NUM_SEGMENTS; ++i) {
        float theta = 2.0f * M_PI * (float)i / (float)NUM_SEGMENTS;
        float x = r * cos(theta);
        float z = r * sin(theta);
        glVertex3f(pos(0) + x, 0.0f, pos(2) + z);
    }

    glEnd();
}
