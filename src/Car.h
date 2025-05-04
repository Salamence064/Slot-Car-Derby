#pragma once

#include <string>
#include <memory>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "MatrixStack.h"
#include "Shape.h"
#include "Particle.h"
#include "Program.h"

class Car {
    private:
        const glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f); // the up direction in the model

        std::shared_ptr<Shape> car;
        std::shared_ptr<Particle> slot; // particle representing the slot for the slot car

        glm::quat rot_offset; // rotation specifically to make the model's vertical axis point up
        glm::vec3 translation; // translation for the car with respect to the slot
        glm::quat rotation; // rotation of the car with respect to the slot
        float scale; // scale of the car -- the car model is way too big so we'll just use this to scale it down

    public:
        Car(const std::string &resourceDir, std::shared_ptr<Particle> slotParticle);
        ~Car();

        void align_car(glm::vec3 const &dir); // rotate the car to align with the direction the slot particle is moving in
        void draw(std::shared_ptr<MatrixStack> MV, std::shared_ptr<Program> prog) const;
};
