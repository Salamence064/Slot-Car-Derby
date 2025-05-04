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
    rot_offset = glm::quat(glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(-1.0f, 0.0f, 0.0f)));
    rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f); // Initialize rotation to identity
}

Car::~Car() {
}

void Car::align_car(glm::vec3 const &dir) {
    glm::vec3 d = glm::normalize(dir); // Normalize the direction vector
    rotation = glm::rotation(forward, d); // Compute the rotation quaternion to align the car with the direction vector
}


void Car::draw(std::shared_ptr<MatrixStack> MV, std::shared_ptr<Program> prog) const {
    slot->draw(MV, prog); // Draw the slot particle

    // Apply the translation and rotation to the model-view matrix
    MV->pushMatrix();

        MV->translate(translation.x + slot->x(0), translation.y + slot->x(1), translation.z + slot->x(2)); // Translate to the car's position
        MV->multMatrix(glm::mat4(rot_offset)); // Apply the rotation offset to make the model's vertical axis point up
        MV->multMatrix(glm::mat4(rotation)); // Apply the rotation
        MV->scale(scale); // Scale the car

        glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, glm::value_ptr(MV->topMatrix()));

        // Draw the car shape
        car->draw(prog);

    MV->popMatrix(); // Restore the previous matrix state
}
