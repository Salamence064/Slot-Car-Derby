#include "ground.h"

#include <iostream>
#include <cmath>

#include <glm/gtc/type_ptr.hpp>

using namespace std;

Ground::Ground() :
    posBuf(),
    norBuf(),
    texBuf(),
    indBuf(),
    bufIDs(),
    indCount(0)
{
    restitution = 0.66f; // restitution coefficient for the ground
    damping = 0.8f; // friction coefficient for the ground
    y = -1.0f; // y coordinate of the ground
}

Ground::~Ground()
{
}

void Ground::init()
{
    float minX = -200.0f, maxX = 200.0f;
    float minZ = -200.0f, maxZ = 200.0f;

    // Vert 0
	posBuf.push_back(minX);
	posBuf.push_back(y);
	posBuf.push_back(minZ);
	norBuf.push_back(0.0f);
	norBuf.push_back(1.0f);
	norBuf.push_back(0.0f);

	// Vert 1
	posBuf.push_back(maxX);
	posBuf.push_back(y);
	posBuf.push_back(minZ);
	norBuf.push_back(0.0f);
	norBuf.push_back(1.0f);
	norBuf.push_back(0.0f);

	// Vert 2
	posBuf.push_back(minX);
	posBuf.push_back(y);
	posBuf.push_back(maxZ);
	norBuf.push_back(0.0f);
	norBuf.push_back(1.0f);
	norBuf.push_back(0.0f);

	// Vert 3
	posBuf.push_back(maxX);
	posBuf.push_back(y);
	posBuf.push_back(maxZ);
	norBuf.push_back(0.0f);
	norBuf.push_back(1.0f);
	norBuf.push_back(0.0f);

	// indices
	indBuf.push_back(0);
	indBuf.push_back(1);
	indBuf.push_back(3);
	indBuf.push_back(0);
	indBuf.push_back(3);
	indBuf.push_back(2);

    // Total number of indices
    indCount = (int)indBuf.size();

    // Generate buffer IDs and put them in the bufIDs map.
    vector<GLuint> tmp(3);
    glGenBuffers(3, &tmp[0]);

    bufIDs["bPos"] = tmp[0];
    bufIDs["bNor"] = tmp[1];
    bufIDs["bInd"] = tmp[2];

    glBindBuffer(GL_ARRAY_BUFFER, bufIDs["bPos"]);
    glBufferData(GL_ARRAY_BUFFER, posBuf.size()*sizeof(float), &posBuf[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, bufIDs["bNor"]);
    glBufferData(GL_ARRAY_BUFFER, norBuf.size()*sizeof(float), &norBuf[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufIDs["bInd"]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indBuf.size()*sizeof(unsigned int), &indBuf[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    assert(norBuf.size() == posBuf.size());

    GLSL::checkError(GET_FILE_LINE);
}

void Ground::draw(
    std::shared_ptr<Program> prog,
    std::shared_ptr<MatrixStack> MV
) const
{
    MV->pushMatrix();

        glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, glm::value_ptr(MV->topMatrix()));

        // ---------------------------------------------------------------------------------------
        // Draw the ground

        glEnableVertexAttribArray(prog->getAttribute("aPos"));
        GLSL::checkError(GET_FILE_LINE);
        glEnableVertexAttribArray(prog->getAttribute("aNor"));
        GLSL::checkError(GET_FILE_LINE);

        glBindBuffer(GL_ARRAY_BUFFER, bufIDs.at("bPos"));
        glVertexAttribPointer(prog->getAttribute("aPos"), 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

        glBindBuffer(GL_ARRAY_BUFFER, bufIDs.at("bNor"));
        glVertexAttribPointer(prog->getAttribute("aNor"), 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufIDs.at("bInd"));
        glDrawElements(GL_TRIANGLES, indCount, GL_UNSIGNED_INT, (void *)0);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glDisableVertexAttribArray(prog->getAttribute("aNor"));
        glDisableVertexAttribArray(prog->getAttribute("aPos"));

        GLSL::checkError(GET_FILE_LINE);

        // ---------------------------------------------------------------------------------------

    MV->popMatrix();
}
