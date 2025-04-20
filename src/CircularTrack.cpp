#include "CircularTrack.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

CircularTrack::CircularTrack(Eigen::Vector3d pos, float scale, double radius) : Track() {
    r = radius; // set the radius of the circular track
    this->pos = pos; // set the position of the circular track
    this->scale = scale; // set the scale of the circular track
}

// todo could maybe update to use progSimple instead
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
