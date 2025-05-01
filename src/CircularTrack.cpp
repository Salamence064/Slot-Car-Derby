#include "CircularTrack.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

CircularTrack::CircularTrack(Eigen::Vector3d pos, float scale, double radius) : Track() {
    r = radius; // set the radius of the circular track
    this->pos = pos; // set the position of the circular track
    this->scale = scale; // set the scale of the circular track
}

void CircularTrack::draw(std::shared_ptr<MatrixStack> MV, const std::shared_ptr<Program> prog) const {
    MV->pushMatrix();
    
        MV->translate(pos(0), pos(1), pos(2)); // translate to the position of the circular track
        MV->scale(scale); // scale the circular track

        glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, glm::value_ptr(MV->topMatrix()));

        glBegin(GL_LINE_LOOP);

            for (int i = 0; i < NUM_SEGMENTS; ++i) {
                // calculate the angle for the current segment
                float theta = 2.0f * M_PI * (float)i / (float)NUM_SEGMENTS;

                // calculate the x, y, z coordinates of the vertex on the circular path
                float x = r * cos(theta);
                float y = 0.0f; // y is constant
                float z = r * sin(theta);

                // draw the vertex
                glVertex3f(x, y, z);
            }

        glEnd();

    MV->popMatrix();
}

double CircularTrack::C(Eigen::Vector3d x) const {
    return (x - pos).norm() - r; // distance from the center of the track to the point x should be equal to the radius
}

double CircularTrack::Cn(Eigen::Vector3d x) const {
    return 0.0; // normal constraint function is not needed for circular track as it is flat
}

Eigen::Vector3d CircularTrack::gradC(Eigen::Vector3d x) const {
    return (x - pos).normalized(); // gradient of the constraint function is the normalized vector from the center of the track to the point x
}

Eigen::Vector3d CircularTrack::gradCn(Eigen::Vector3d x) const {
    return Eigen::Vector3d(1.0, 0.0, 0.0); // gradient of the normal constraint function is not needed for circular track
}

Eigen::Vector3d CircularTrack::getForward(Eigen::Vector3d x) const {
    // calculate the angle of the point x on the circular track
    float theta = atan2(x(2) - pos(2), x(0) - pos(0)); // angle in radians

    // calculate the forward direction based on the angle
    return Eigen::Vector3d(-sin(theta), 0.0, cos(theta)); // forward direction is tangent to the circle at point x
}
