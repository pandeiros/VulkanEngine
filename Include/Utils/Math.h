/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#pragma once

#include "VulkanCore.h"

#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "glm/gtc/matrix_transform.hpp"

#ifdef __ANDROID__
#include "gvr_types.h"
#endif

#include <algorithm>

/**
 * @file Math.h
 */

constexpr double PI = 3.14159265359;

VULKAN_NS_BEGIN

struct Vector2D
{
    Vector2D()
        : x(0.f), y(0.f)
    {}

    Vector2D(float x, float y)
        : x(x), y(y)
    {}

    float x, y;
};

struct Transform
{
    Transform()
    {
        position = glm::vec3(0.f, 0.f, 0.f);
        scale = glm::vec3(1.f, 1.f, 1.f);
    }

    Transform(glm::vec3 position, glm::vec3 scale)
        : position(position), scale(scale)
    {}

    const Transform operator+(const Transform& other) const
    {
        Transform result = *this;
        result.position += other.position;
        result.scale *= other.scale;

        return result;
    }

    glm::vec3 position;
    glm::vec3 scale;
};

/**
 * @class Math
 */
class Math
{
public:
    /**
     * Return A - B, which is a vector that contains elements from vector A that are not present in vector B.
     */
    template<typename T>
    static std::vector<T> Diff(const std::vector<T>& a, const std::vector<T>& b);

    template<typename T>
    static T Clamp(const T& Value, const T&Min, const T&Max);

    static float MapToRange(const float value, const float inputMin, const float inputMax, const float outputMin, const float outputMax);

#ifdef __ANDROID__
    static glm::mat4 Transpose(gvr::Mat4f mat4)
    {
        glm::mat4 glmMatrix;
        glmMatrix[0] = glm::vec4(mat4.m[0][0], mat4.m[1][0], mat4.m[2][0], mat4.m[3][0]);
        glmMatrix[1] = glm::vec4(mat4.m[0][1], mat4.m[1][1], mat4.m[2][1], mat4.m[3][1]);;
        glmMatrix[2] = glm::vec4(mat4.m[0][2], mat4.m[1][2], mat4.m[2][2], mat4.m[3][2]);;
        glmMatrix[3] = glm::vec4(mat4.m[0][3], mat4.m[1][3], mat4.m[2][3], mat4.m[3][3]);;
        //glmMatrix[0] = glm::vec4(mat4.m[0][0], mat4.m[0][1], mat4.m[0][2], mat4.m[0][3]);
        //glmMatrix[1] = glm::vec4(mat4.m[1][0], mat4.m[1][1], mat4.m[1][2], mat4.m[1][3]);;
        //glmMatrix[2] = glm::vec4(mat4.m[2][0], mat4.m[2][1], mat4.m[2][2], mat4.m[2][3]);;
        //glmMatrix[3] = glm::vec4(mat4.m[3][0], mat4.m[3][1], mat4.m[3][2], mat4.m[3][3]);;

        return glmMatrix;
    }
#endif
private:

};

template<typename T>
std::vector<T> Math::Diff(const std::vector<T>& a, const std::vector<T>& b)
{
    std::vector<T> diff;
    for (size_t i = 0; i < a.size(); ++i)
    {
        T element = a[i];
        if (std::find(b.begin(), b.end(), element) == b.end())
        {
            diff.push_back(element);
        }
    }

    return diff;
}

template<>
inline std::vector<const char*> Math::Diff<const char*>(const std::vector<const char*>& a, const std::vector<const char*>& b)
{
    std::vector<const char*> diff;
    for (size_t i = 0; i < a.size(); ++i)
    {
        const char* element = a[i];
        if (std::find_if(b.begin(), b.end(), [element](const char* c)
        {
            return strcmp(c, element) == 0;
        }) == b.end())
        {
            diff.push_back(element);
        }
    }

    return diff;
}

template<typename T>
T Math::Clamp(const T& Value, const T&Min, const T&Max)
{
    if (Value < Min)
    {
        return Min;
    }
    else if (Value > Max)
    {
        return Max;
    }

    return Value;
}

VULKAN_NS_END