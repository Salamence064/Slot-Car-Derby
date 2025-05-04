#include "Car.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include <iostream>

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
}

Car::~Car() {
}

void Car::align_car(glm::vec3 const &dir) {
    glm::vec3 f = glm::normalize(dir); // Normalize the direction vector
    glm::vec3 r = glm::normalize(glm::cross(up, f)); // Right vector
    glm::vec3 u = glm::normalize(glm::cross(f, r)); // "True" up

    glm::mat3 basis(r, u, f);
    rotation = glm::quat_cast(basis); // Convert the basis matrix to a quaternion
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
