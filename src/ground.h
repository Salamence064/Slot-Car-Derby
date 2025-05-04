#pragma once

#include <memory>
#include <vector>
#include <string>
#include <map>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "GLSL.h"
#include "MatrixStack.h"
#include "Program.h"


class Ground {
    private:
        std::vector<float> posBuf;
        std::vector<float> norBuf;
        std::vector<float> texBuf;
        std::vector<unsigned int> indBuf;

        std::map<std::string,GLuint> bufIDs;

        int indCount;

    public:
        float y; // y coordinate of the ground
        float restitution; // restitution coefficient for the ground
        float damping; // friction coefficient for the ground

        Ground();
        ~Ground();

        void init();

        void draw(
            std::shared_ptr<Program> prog,
            std::shared_ptr<MatrixStack> MV
        ) const;
};
