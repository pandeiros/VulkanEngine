/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#include "Utils/Math.h"

VULKAN_NS_USING;

float Math::MapToRange(const float value, const float inputMin, const float inputMax, const float outputMin, const float outputMax)
{
    return (value - inputMin) / (inputMax - inputMin) * (outputMax - outputMin) + outputMin;
}

#ifdef __ANDROID__
glm::mat4 Math::Transpose(gvr::Mat4f mat4)
{
    glm::mat4 glmMatrix;
    //glmMatrix[0] = glm::vec4(mat4.m[0][0], mat4.m[1][0], mat4.m[2][0], mat4.m[3][0]);
    //glmMatrix[1] = glm::vec4(mat4.m[0][1], mat4.m[1][1], mat4.m[2][1], mat4.m[3][1]);
    //glmMatrix[2] = glm::vec4(mat4.m[0][2], mat4.m[1][2], mat4.m[2][2], mat4.m[3][2]);
    //glmMatrix[3] = glm::vec4(mat4.m[0][3], mat4.m[1][3], mat4.m[2][3], mat4.m[3][3]);

    // #TODO Temporary fix
    glmMatrix[0] = glm::vec4(mat4.m[0][1], mat4.m[1][1], mat4.m[2][1], mat4.m[3][1]);
    glmMatrix[1] = -1.f * glm::vec4(mat4.m[0][0], mat4.m[1][0], mat4.m[2][0], mat4.m[3][0]);
    glmMatrix[2] = glm::vec4(mat4.m[0][2], mat4.m[1][2], mat4.m[2][2], mat4.m[3][2]);
    glmMatrix[3] = glm::vec4(mat4.m[0][3], mat4.m[1][3], mat4.m[2][3], mat4.m[3][3]);


    //glmMatrix[0] = glm::vec4(mat4.m[0][0], mat4.m[0][1], mat4.m[0][2], mat4.m[0][3]);
    //glmMatrix[1] = glm::vec4(mat4.m[1][0], mat4.m[1][1], mat4.m[1][2], mat4.m[1][3]);
    //glmMatrix[2] = glm::vec4(mat4.m[2][0], mat4.m[2][1], mat4.m[2][2], mat4.m[2][3]);
    //glmMatrix[3] = glm::vec4(mat4.m[3][0], mat4.m[3][1], mat4.m[3][2], mat4.m[3][3]);

    return glmMatrix;
}
#endif
