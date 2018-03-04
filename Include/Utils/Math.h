/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2018 Pawel Kaczynski
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
        rotation = glm::vec3(0.f, 0.f, 0.f);
    }

    Transform(glm::vec3 position, glm::vec3 scale, glm::vec3 rotation = glm::vec3(0.f, 0.f, 0.f))
        : position(position), scale(scale), rotation(rotation)
    {}

    const Transform operator+(const Transform& other) const
    {
        Transform result = *this;
        result.position += other.position;
        result.scale *= other.scale;
        result.rotation += other.rotation;

        return result;
    }

    glm::vec3 position;
    glm::vec3 scale;
    glm::vec3 rotation;
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
    static glm::mat4 Transpose(gvr::Mat4f mat4);
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