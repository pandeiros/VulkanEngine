/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#pragma once

#include "VulkanCore.h"

#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

VULKAN_NS_BEGIN

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
    static std::vector<T> Diff(const std::vector<T>& a, const std::vector<T>& b)
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

    template<typename T>
    static void Print(const T& container)
    {
        for (auto & element : container)
        {
            Logger::Log(element);
        }
    }

private:

};

VULKAN_NS_END