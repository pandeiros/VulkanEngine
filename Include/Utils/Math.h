/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#pragma once

#include "VulkanCore.h"

#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>

#include <algorithm>

/**
 * @file Math.h
 */

constexpr double PI = 3.14159265359;

VULKAN_NS_BEGIN

struct Vector2D
{
    float x, y;
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