#include "Car.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include <iostream>

void Car::updateMinMax() {
    glm::vec3 v1 = glm::vec3(car->minX, car->minY, car->minZ);
    glm::vec3 v2 = glm::vec3(car->minX, car->minY, car->maxZ);
    glm::vec3 v3 = glm::vec3(car->minX, car->maxY, car->minZ);
    glm::vec3 v4 = glm::vec3(car->minX, car->maxY, car->maxZ);

    glm::vec3 v5 = glm::vec3(car->maxX, car->minY, car->minZ);
    glm::vec3 v6 = glm::vec3(car->maxX, car->minY, car->maxZ);
    glm::vec3 v7 = glm::vec3(car->maxX, car->maxY, car->minZ);
    glm::vec3 v8 = glm::vec3(car->maxX, car->maxY, car->maxZ);


    auto M = std::make_shared<MatrixStack>();
    M->loadIdentity(); // Load the identity matrix

    M->pushMatrix();

        M->translate(translation.x + slot->x(0), translation.y + slot->x(1), translation.z + slot->x(2)); // Translate to the car's position
        glm::quat full_rot = rotation * rot_offset; // Combine the rotation with the offset
        M->multMatrix(glm::mat4(full_rot)); // Apply the rotation
        M->scale(scale); // Scale the car

        // Transform the vertices
        v1 = glm::vec3(M->topMatrix() * glm::vec4(v1, 1.0f));
        v2 = glm::vec3(M->topMatrix() * glm::vec4(v2, 1.0f));
        v3 = glm::vec3(M->topMatrix() * glm::vec4(v3, 1.0f));
        v4 = glm::vec3(M->topMatrix() * glm::vec4(v4, 1.0f));

        v5 = glm::vec3(M->topMatrix() * glm::vec4(v5, 1.0f));
        v6 = glm::vec3(M->topMatrix() * glm::vec4(v6, 1.0f));
        v7 = glm::vec3(M->topMatrix() * glm::vec4(v7, 1.0f));
        v8 = glm::vec3(M->topMatrix() * glm::vec4(v8, 1.0f));

    M->popMatrix();

    // determine the min and max vertices of the bounding box
    minVertex.x = std::min({v1.x, v2.x, v3.x, v4.x, v5.x, v6.x, v7.x, v8.x});
    minVertex.y = std::min({v1.y, v2.y, v3.y, v4.y, v5.y, v6.y, v7.y, v8.y});
    minVertex.z = std::min({v1.z, v2.z, v3.z, v4.z, v5.z, v6.z, v7.z, v8.z});

    maxVertex.x = std::max({v1.x, v2.x, v3.x, v4.x, v5.x, v6.x, v7.x, v8.x});
    maxVertex.y = std::max({v1.y, v2.y, v3.y, v4.y, v5.y, v6.y, v7.y, v8.y});
    maxVertex.z = std::max({v1.z, v2.z, v3.z, v4.z, v5.z, v6.z, v7.z, v8.z});
}

Car::Car(const std::string &resourceDir, std::shared_ptr<Particle> slotParticle) {
    slot = slotParticle; // Initialize the slot particle

    // Load the car shape from the resource directory
    car = std::make_shared<Shape>();
    car->loadMesh(resourceDir + "/car.obj");
    car->computeMinMax(); // Compute the bounding box of the car shape
    car->init();
    
    // Initialize translation and rotation
    translation = glm::vec3(0.0f, 0.0f, 0.0f);
    scale = 0.01f; // Scale down the car model

    glm::quat yaw90  = glm::angleAxis(glm::radians(-90.f), glm::vec3(0,1,0)); // Rotate 90 degrees around the y-axis
    glm::quat tilt90 = glm::angleAxis(glm::radians(-90.f), glm::vec3(1,0,0)); // Rotate -90 degrees around the x-axis
    rot_offset = yaw90 * tilt90; // Combine the rotations to make the car's vertical axis point up
    rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f); // Initialize rotation to identity

    updateMinMax();
}

Car::~Car() {
}

void Car::align_car(glm::vec3 const &dir) {
    glm::vec3 f = glm::normalize(dir); // Normalize the direction vector
    glm::vec3 r = glm::normalize(glm::cross(up, f)); // Right vector
    glm::vec3 u = glm::normalize(glm::cross(f, r)); // "True" up

    glm::mat3 basis(r, u, f);
    rotation = glm::quat_cast(basis); // Convert the basis matrix to a quaternion

    updateMinMax(); // Update the particles to match the new rotation
}


void Car::draw(std::shared_ptr<MatrixStack> MV, std::shared_ptr<Program> prog) const {
    slot->draw(MV, prog); // Draw the slot particle

    // Apply the translation and rotation to the model-view matrix
    MV->pushMatrix();

        MV->translate(translation.x + slot->x(0), translation.y + slot->x(1), translation.z + slot->x(2)); // Translate to the car's position
        glm::quat full_rot = rotation * rot_offset; // Combine the rotation with the offset
        MV->multMatrix(glm::mat4(full_rot)); // Apply the rotation
        MV->scale(scale); // Scale the car

        glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, glm::value_ptr(MV->topMatrix()));

        // Draw the car shape
        car->draw(prog);

    MV->popMatrix(); // Restore the previous matrix state
}
