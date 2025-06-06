#include "SplineTrack.h"

#include <glm/gtc/type_ptr.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/norm.hpp>

#include <iostream>


SplineTrack::SplineTrack(Eigen::Vector3d pos, float scale) : Track()
{
    this->pos = pos;
    this->scale = scale;
    
    // Catmull-Rom spline basis matrix
    B[3] = glm::vec4(-0.5f, 1.5f, -1.5f, 0.5f);
    B[2] = glm::vec4(1.0f, -2.5f, 2.0f, -0.5f);
    B[1] = glm::vec4(-0.5f, 0, 0.5f, 0);
    B[0] = glm::vec4(0, 1.0f, 0, 0);

    ncps = 0; // number of control points
    cps = std::vector<glm::vec3>(); // control points
}

void SplineTrack::draw(std::shared_ptr<MatrixStack> MV, const std::shared_ptr<Program> prog) const
{
    MV->pushMatrix();
    
        MV->translate(pos(0), pos(1), pos(2)); // translate to the position of the circular track
        MV->scale(scale); // scale the circular track

        glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, glm::value_ptr(MV->topMatrix()));

        glLineWidth(1.0f);
        glColor3f(0.4f, 0.4f, 0.4f);

        glBegin(GL_LINE_STRIP);

        for (int i = 0; i < ncps; ++i) {
            int im1 = (i - 1 + ncps) % ncps; // previous control point index
            int ip0 = i;                     // current control point index
            int ip1 = (i + 1) % ncps;        // next control point index
            int ip2 = (i + 2) % ncps;        // next next control point index

            glm::mat4 G;
            G[0] = glm::vec4(cps[im1], 0.0f);
            G[1] = glm::vec4(cps[ip0], 0.0f);
            G[2] = glm::vec4(cps[ip1], 0.0f);
            G[3] = glm::vec4(cps[ip2], 0.0f);

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
    glm::mat4 G;

    // find the closest point on the spline to the car's position
    float minDistSq = std::numeric_limits<float>::max();
    float u = 0.0f;
    int index = 0;

    for (int i = 0; i < ncps; ++i) {
        int im1 = (i - 1 + ncps) % ncps; // previous control point index
        int ip0 = i;                     // current control point index
        int ip1 = (i + 1) % ncps;        // next control point index
        int ip2 = (i + 2) % ncps;        // next next control point index

        glm::mat4 G;
        G[0] = glm::vec4(cps[im1], 0.0f);
        G[1] = glm::vec4(cps[ip0], 0.0f);
        G[2] = glm::vec4(cps[ip1], 0.0f);
        G[3] = glm::vec4(cps[ip2], 0.0f);

        for (float uTemp = 0.01f; uTemp < 1.0f; uTemp += 0.01f) {
            glm::vec4 uVec(1.0f, uTemp, uTemp*uTemp, uTemp*uTemp*uTemp);
            glm::vec4 p = G*B*uVec;

            float distSq = (x - Eigen::Vector3d(p.x, p.y, p.z)).squaredNorm();
            if (distSq < minDistSq) {
                minDistSq = distSq;
                u = uTemp;
                index = i;
            }
        }
    }

    // compute the tangent vector at the closest point on the spline
    int im1 = (index - 1 + ncps) % ncps; // previous control point index
    int ip0 = index;                     // current control point index
    int ip1 = (index + 1) % ncps;        // next control point index
    int ip2 = (index + 2) % ncps;        // next next control point index

    G[0] = glm::vec4(cps[im1], 0.0f);
    G[1] = glm::vec4(cps[ip0], 0.0f);
    G[2] = glm::vec4(cps[ip1], 0.0f);
    G[3] = glm::vec4(cps[ip2], 0.0f);

    glm::vec4 uPrimeVec(0.0f, 1.0f, 2.0f*u, 3.0f*u*u);
    glm::vec3 tangent = glm::normalize(glm::vec3(G*B*uPrimeVec));

    // compute the normal and bi-normal vector
    glm::vec3 normal = up; // initial normal (up direction)
    glm::vec3 biNormal = glm::cross(tangent, normal); // compute the bi-normal (cross product of tangent and normal)

    // compute p(u)
    glm::vec4 uVec(1.0f, u, u*u, u*u*u);
    glm::vec3 p = glm::vec3(G*B*uVec);

    // constraint function (distance to the track)
    // C(x) = (x - p(u)) * biNormal = 0
    return (x - Eigen::Vector3d(p.x, p.y, p.z)).dot(Eigen::Vector3d(biNormal.x, biNormal.y, biNormal.z));
}

double SplineTrack::Cn(Eigen::Vector3d x) const
{
    glm::mat4 G;

    // find the closest point on the spline to the car's position
    float minDistSq = std::numeric_limits<float>::max();
    float u = 0.0f;
    int index = 0;

    for (int i = 0; i < ncps; ++i) {
        int im1 = (i - 1 + ncps) % ncps; // previous control point index
        int ip0 = i;                     // current control point index
        int ip1 = (i + 1) % ncps;        // next control point index
        int ip2 = (i + 2) % ncps;        // next next control point index

        glm::mat4 G;
        G[0] = glm::vec4(cps[im1], 0.0f);
        G[1] = glm::vec4(cps[ip0], 0.0f);
        G[2] = glm::vec4(cps[ip1], 0.0f);
        G[3] = glm::vec4(cps[ip2], 0.0f);

        for (float uTemp = 0.01f; uTemp < 1.0f; uTemp += 0.01f) {
            glm::vec4 uVec(1.0f, uTemp, uTemp*uTemp, uTemp*uTemp*uTemp);
            glm::vec4 p = G*B*uVec;

            float distSq = (x - Eigen::Vector3d(p.x, p.y, p.z)).squaredNorm();
            if (distSq < minDistSq) {
                minDistSq = distSq;
                u = uTemp;
                index = i;
            }
        }
    }

    // compute the tangent vector at the closest point on the spline
    int im1 = (index - 1 + ncps) % ncps; // previous control point index
    int ip0 = index;                     // current control point index
    int ip1 = (index + 1) % ncps;        // next control point index
    int ip2 = (index + 2) % ncps;        // next next control point index

    G[0] = glm::vec4(cps[im1], 0.0f);
    G[1] = glm::vec4(cps[ip0], 0.0f);
    G[2] = glm::vec4(cps[ip1], 0.0f);
    G[3] = glm::vec4(cps[ip2], 0.0f);

    // compute the normal vector
    glm::vec3 normal = up; // initial normal (up direction)

    // compute p(u)
    glm::vec4 uVec(1.0f, u, u*u, u*u*u);
    glm::vec3 p = glm::vec3(G*B*uVec);

    // constraint function (distance to the track)
    // C(x) = (x - p(u)) * normal = 0
    return (x - Eigen::Vector3d(p.x, p.y, p.z)).dot(Eigen::Vector3d(normal.x, normal.y, normal.z));
}

Eigen::Vector3d SplineTrack::gradC(Eigen::Vector3d x) const
{
    glm::mat4 G;

    // find the closest point on the spline to the car's position
    float minDistSq = std::numeric_limits<float>::max();
    float u = 0.0f;
    int index = 0;

    for (int i = 0; i < ncps; ++i) {
        int im1 = (i - 1 + ncps) % ncps; // previous control point index
        int ip0 = i;                     // current control point index
        int ip1 = (i + 1) % ncps;        // next control point index
        int ip2 = (i + 2) % ncps;        // next next control point index

        glm::mat4 G;
        G[0] = glm::vec4(cps[im1], 0.0f);
        G[1] = glm::vec4(cps[ip0], 0.0f);
        G[2] = glm::vec4(cps[ip1], 0.0f);
        G[3] = glm::vec4(cps[ip2], 0.0f);

        for (float uTemp = 0.01f; uTemp < 1.0f; uTemp += 0.01f) {
            glm::vec4 uVec(1.0f, uTemp, uTemp*uTemp, uTemp*uTemp*uTemp);
            glm::vec4 p = G*B*uVec;

            float distSq = (x - Eigen::Vector3d(p.x, p.y, p.z)).squaredNorm();
            if (distSq < minDistSq) {
                minDistSq = distSq;
                u = uTemp;
                index = i;
            }
        }
    }

    // compute the tangent vector at the closest point on the spline
    int im1 = (index - 1 + ncps) % ncps; // previous control point index
    int ip0 = index;                     // current control point index
    int ip1 = (index + 1) % ncps;        // next control point index
    int ip2 = (index + 2) % ncps;        // next next control point index

    G[0] = glm::vec4(cps[im1], 0.0f);
    G[1] = glm::vec4(cps[ip0], 0.0f);
    G[2] = glm::vec4(cps[ip1], 0.0f);
    G[3] = glm::vec4(cps[ip2], 0.0f);

    glm::vec4 uPrimeVec(0.0f, 1.0f, 2.0f*u, 3.0f*u*u);
    glm::vec3 tangent = glm::normalize(glm::vec3(G*B*uPrimeVec));

    // compute the normal and bi-normal vector
    glm::vec3 normal = up; // initial normal (up direction)
    glm::vec3 biNormal = glm::cross(tangent, normal); // compute the bi-normal (cross product of tangent and normal)

    return Eigen::Vector3d(biNormal.x, biNormal.y, biNormal.z); // gradient of the constraint function (bi-normal vector)
}

Eigen::Vector3d SplineTrack::gradCn(Eigen::Vector3d x) const
{
    return Eigen::Vector3d(up.x, up.y, up.z); // gradient of the constraint function (normal vector)
}

Eigen::Vector3d SplineTrack::getForward(Eigen::Vector3d x) const
{
    glm::mat4 G;

    // find the closest point on the spline to the car's position
    float minDistSq = std::numeric_limits<float>::max();
    float u = 0.0f;
    int index = 0;

    for (int i = 0; i < ncps; ++i) {
        int im1 = (i - 1 + ncps) % ncps; // previous control point index
        int ip0 = i;                     // current control point index
        int ip1 = (i + 1) % ncps;        // next control point index
        int ip2 = (i + 2) % ncps;        // next next control point index

        glm::mat4 G;
        G[0] = glm::vec4(cps[im1], 0.0f);
        G[1] = glm::vec4(cps[ip0], 0.0f);
        G[2] = glm::vec4(cps[ip1], 0.0f);
        G[3] = glm::vec4(cps[ip2], 0.0f);

        for (float uTemp = 0.01f; uTemp < 1.0f; uTemp += 0.01f) {
            glm::vec4 uVec(1.0f, uTemp, uTemp*uTemp, uTemp*uTemp*uTemp);
            glm::vec4 p = G*B*uVec;

            float distSq = (x - Eigen::Vector3d(p.x, p.y, p.z)).squaredNorm();
            if (distSq < minDistSq) {
                minDistSq = distSq;
                u = uTemp;
                index = i;
            }
        }
    }

    // compute the tangent vector at the closest point on the spline
    int im1 = (index - 1 + ncps) % ncps; // previous control point index
    int ip0 = index;                     // current control point index
    int ip1 = (index + 1) % ncps;        // next control point index
    int ip2 = (index + 2) % ncps;        // next next control point index

    G[0] = glm::vec4(cps[im1], 0.0f);
    G[1] = glm::vec4(cps[ip0], 0.0f);
    G[2] = glm::vec4(cps[ip1], 0.0f);
    G[3] = glm::vec4(cps[ip2], 0.0f);

    glm::vec4 uPrimeVec(0.0f, 1.0f, 2.0f*u, 3.0f*u*u);
    glm::vec3 tangent = glm::normalize(glm::vec3(G*B*uPrimeVec));

    return Eigen::Vector3d(tangent.x, tangent.y, tangent.z); // forward direction of the track at the given point
}

double SplineTrack::getCurvature(Eigen::Vector3d x) const
{
    glm::mat4 G;

    // find the closest point on the spline to the car's position
    float minDistSq = std::numeric_limits<float>::max();
    float u = 0.0f;
    int index = 0;

    for (int i = 0; i < ncps; ++i) {
        int im1 = (i - 1 + ncps) % ncps; // previous control point index
        int ip0 = i;                     // current control point index
        int ip1 = (i + 1) % ncps;        // next control point index
        int ip2 = (i + 2) % ncps;        // next next control point index

        glm::mat4 G;
        G[0] = glm::vec4(cps[im1], 0.0f);
        G[1] = glm::vec4(cps[ip0], 0.0f);
        G[2] = glm::vec4(cps[ip1], 0.0f);
        G[3] = glm::vec4(cps[ip2], 0.0f);

        for (float uTemp = 0.01f; uTemp < 1.0f; uTemp += 0.01f) {
            glm::vec4 uVec(1.0f, uTemp, uTemp*uTemp, uTemp*uTemp*uTemp);
            glm::vec4 p = G*B*uVec;

            float distSq = (x - Eigen::Vector3d(p.x, p.y, p.z)).squaredNorm();
            if (distSq < minDistSq) {
                minDistSq = distSq;
                u = uTemp;
                index = i;
            }
        }
    }

    // compute the tangent vector at the closest point on the spline
    int im1 = (index - 1 + ncps) % ncps; // previous control point index
    int ip0 = index;                     // current control point index
    int ip1 = (index + 1) % ncps;        // next control point index
    int ip2 = (index + 2) % ncps;        // next next control point index

    G[0] = glm::vec4(cps[im1], 0.0f);
    G[1] = glm::vec4(cps[ip0], 0.0f);
    G[2] = glm::vec4(cps[ip1], 0.0f);
    G[3] = glm::vec4(cps[ip2], 0.0f);

    // first derivative
    glm::vec4 uPrimeVec(0.0f, 1.0f, 2.0f*u, 3.0f*u*u);
    glm::vec3 pPrime = glm::vec3(G*B*uPrimeVec);

    // second derivative
    glm::vec4 uDoublePrimeVec(0.0f, 0.0f, 2.0f, 6.0f*u);
    glm::vec3 pDoublePrime = glm::vec3(G*B*uDoublePrimeVec);

    // curvature of the track at the given point
    return glm::length(glm::cross(pPrime, pDoublePrime)) / pow(glm::length(pPrime), 3);
}
