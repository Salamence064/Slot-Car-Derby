#include "SplineTrack.h"

#include <glm/gtc/type_ptr.hpp>


SplineTrack::SplineTrack(Eigen::Vector3d pos, float scale) : Track()
{
    this->pos = pos;
    this->scale = scale;
    
    // Catmull-Rom spline basis matrix
    B[3] = glm::vec4(-0.5f, 1.5f, -1.5f, 0.5f);
    B[2] = glm::vec4(1.0f, -2.5f, 2.0f, -0.5f);
    B[1] = glm::vec4(-0.5f, 0, 0.5f, 0);
    B[0] = glm::vec4(0, 1.0f, 0, 0);
}

void SplineTrack::draw(std::shared_ptr<MatrixStack> MV, const std::shared_ptr<Program> prog) const
{
    MV->pushMatrix();
    
        MV->translate(pos(0), pos(1), pos(2)); // translate to the position of the circular track
        MV->scale(scale); // scale the circular track

        glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, glm::value_ptr(MV->topMatrix()));

        glLineWidth(1.0f);

        glBegin(GL_LINE_STRIP);

        for (int i = 0; i < cps.size() - 3; ++i) {
            glm::mat4 G;
            G[0] = glm::vec4(cps[i],     0.0f);
            G[1] = glm::vec4(cps[i + 1], 0.0f);
            G[2] = glm::vec4(cps[i + 2], 0.0f);
            G[3] = glm::vec4(cps[i + 3], 0.0f);

            for (float u = 0.01f; u < 1.0f; u += 0.01f) {
                glm::vec4 uVec(1.0f, u, u*u, u*u*u);
                glm::vec4 p = G*B*uVec;
                glVertex3f(p.x, p.y, p.z);
            }
        }

        glEnd();

    MV->popMatrix();
}

double SplineTrack::C(Eigen::Vector3d x) const
{
    // // Compute the closest point on the spline to the point x
    // double minDist = std::numeric_limits<double>::max();
    // for (int i = 0; i < cps.size() - 3; ++i) {
    //     glm::mat4 G;
    //     G[0] = glm::vec4(cps[i],     0.0f);
    //     G[1] = glm::vec4(cps[i + 1], 0.0f);
    //     G[2] = glm::vec4(cps[i + 2], 0.0f);
    //     G[3] = glm::vec4(cps[i + 3], 0.0f);

    //     for (float u = 0.01f; u < 1.0f; u += 0.01f) {
    //         glm::vec4 uVec(1.0f, u, u*u, u*u*u);
    //         glm::vec4 p = G*B*uVec;
    //         double dist = (x - Eigen::Vector3d(p.x, p.y, p.z)).norm();
    //         if (dist < minDist) {
    //             minDist = dist;
    //         }
    //     }
    // }
    // return minDist;
    return 0.0; // todo Placeholder, implement the actual constraint function
}

Eigen::Vector3d SplineTrack::gradC(Eigen::Vector3d x) const
{
    // Compute the gradient of the constraint function
    // This is a placeholder implementation, you should replace it with the actual gradient computation
    return Eigen::Vector3d(0.0, 0.0, 0.0); // todo Placeholder, implement the actual gradient of the constraint function
}
