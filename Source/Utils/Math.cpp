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