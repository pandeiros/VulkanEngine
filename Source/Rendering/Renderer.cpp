/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#include "Rendering/Renderer.h"

#include "Core.h"
//#include "Rendering/ShaderTools.h"
#include "Engine.h"

#include <functional>

VULKAN_NS_USING;

VK_DECLARE_LOG_CATEGORY(LogRenderer);

Renderer::Renderer(std::shared_ptr<Device> device, Engine* engine)
    : VulkanClass(device), engine(engine)
{
    bIncludeVertexInput = true;

    uniformBuffers.resize(VULKAN_DESCRIPTOR_SETS_COUNT);

    commandPool.Create(device->GetVkDevice(), VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        device->GetPhysicalDevice()->GetGraphicsQueueFamilyIndex());

    commandPool.AllocateCommandBuffers(device->GetVkDevice(), VK_COMMAND_BUFFER_LEVEL_PRIMARY, VULKAN_COMMAND_BUFFER_COUNT);
}

Renderer::~Renderer()
{
    commandPool.Destroy(device->GetVkDevice());

    vkDestroyPipeline(device->GetVkDevice(), pipeline, nullptr);
    vkDestroyPipelineCache(device->GetVkDevice(), pipelineCache, nullptr);
    vkDestroyDescriptorPool(device->GetVkDevice(), descriptorPool, nullptr);

    vertexBuffer.Destroy(device->GetVkDevice());

    for (VkPipelineShaderStageCreateInfo& shaderStage : pipelineShaderStageCreateInfo)
    {
        vkDestroyShaderModule(device->GetVkDevice(), shaderStage.module, nullptr);
    }

    for (VkDescriptorSetLayout& descriptorSetLayout : descriptorSetLayouts)
    {
        vkDestroyDescriptorSetLayout(device->GetVkDevice(), descriptorSetLayout, nullptr);
    }

    vkDestroyPipelineLayout(device->GetVkDevice(), pipelineLayout, nullptr);

    for (Buffer& uniformBuffer : uniformBuffers)
    {
        uniformBuffer.Destroy(device->GetVkDevice());
    }
}

void Renderer::Init()
{
    VK_PERFORMANCE_SECTION("Renderer init");

    for (uint32_t i = 0; i < VULKAN_DESCRIPTOR_SETS_COUNT; ++i)
    {
        // #TODO Use device properties instead of 256
        uniformBuffers[i].CreateExclusive(device->GetVkDevice(), 0, 2 * 256, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
        uniformBuffers[i].Allocate(device.get(), VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        uniformBuffers[i].UpdateDescriptorInfo(0, VK_WHOLE_SIZE);
    }

    CreateDescriptorSetLayout();
    CreatePipelineLayout();

    SetUpdateEnabled(true);
}

void Renderer::Tick(float deltaTime)
{
    VK_PERFORMANCE_SECTION("Renderer update");

    // 1. If needed, rebuild vertex buffer.

    // Pipeline update.
    if (engine->GetWorld()->IsDirty())
    {
        ShaderIndexData shaderIndexData;
        if (engine->GetWorld()->PrepareVertexData(vertexBuffer, shaderIndexData))
        {
            InitShaders(shaderIndexData);

            AddVertexInputBinding(0, vertexBuffer.GetStride(), VK_VERTEX_INPUT_RATE_VERTEX);
            AddVertexInputAttribute(0, 0, VK_FORMAT_R32G32B32A32_SFLOAT, 0);
            // #TODO Change format when using texture.
            AddVertexInputAttribute(1, 0, VK_FORMAT_R32G32B32A32_SFLOAT, 16);

            InitDescriptorPool();
            InitDescriptorSet();
            InitPipelineCache();
            InitPipeline(engine->GetWindow()->GetSurfaceSize(), engine->GetWindow()->GetRenderPass());
        }
    }

    // Render
    Window* window = engine->GetWindow();
    Queue& queue = device->GetQueueRef();

    if (window->Update())
    {
        std::vector<CommandBuffer>& commandBuffers = commandPool.GetCommandBuffers();

        window->BeginRender();

        std::vector<uint32_t> commandBufferIndexes = {};        // #TODO Refactor these.
        for (uint32_t i = 0; i < commandBuffers.size(); ++i)
        {
            commandBufferIndexes.push_back(i);

            //glm::mat4 mvpMatrix = engine->GetWorld()->GetCameraMatrix(i);
            glm::mat4 viewMatrix = engine->GetWorld()->GetCamera(i)->GetViewMatrix();
            glm::mat4 projectionMatrix = engine->GetWorld()->GetCamera(i)->GetCorrectedProjectionMatrix();
            glm::vec4 undistortionCoefficients = engine->GetWorld()->GetCamera(i)->GetLensUndistortionCoefficients();

            uint32_t offset = i * 256;

            Buffer& uniformBuffer = GetUniformBuffer(0); // #TODO Refactor
            uniformBuffer.Copy(device->GetVkDevice(), &viewMatrix, offset, sizeof(viewMatrix));

            offset += sizeof(viewMatrix);
            uniformBuffer.Copy(device->GetVkDevice(), &projectionMatrix, offset, sizeof(projectionMatrix));

            offset += sizeof(projectionMatrix);
            uniformBuffer.Copy(device->GetVkDevice(), &undistortionCoefficients, offset, sizeof(undistortionCoefficients));

            commandBuffers[i].Reset(0);
            commandBuffers[i].Begin(0, nullptr);

            {
                VK_PERFORMANCE_SECTION("Render initialization");
                uint32_t dynamicOffset = i * 256;
                commandBuffers[i].BeginRenderPass(window->GetRenderPass(), window->GetActiveFramebuffer(), GetRenderArea(i), GetClearValues(), VK_SUBPASS_CONTENTS_INLINE);
                BindPipeline(commandBuffers[i].GetVkCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS);
                BindDescriptorSets(commandBuffers[i].GetVkCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, { dynamicOffset });
                BindVertexBuffers(commandBuffers[i].GetVkCommandBuffer(), { 0 });
                CommandSetViewports(commandBuffers[i].GetVkCommandBuffer());
                CommandSetScissors(commandBuffers[i].GetVkCommandBuffer(), i);
            }

            // #REFACTOR
            {
                VK_PERFORMANCE_SECTION("Draw");
                vkCmdDraw(commandBuffers[i].GetVkCommandBuffer(), vertexBuffer.GetVertexCount(), 1, 0, 0);
            }

            commandBuffers[i].EndRenderPass();
            commandBuffers[i].End();
        }

        // #TODO Clean this up
        VkFence fence = window->GetFence();
        vkResetFences(device->GetVkDevice(), 1, &fence);

        //std::vector<VkCommandBuffer> commandBuffers = { commandBuffer.GetVkCommandBuffer() };
        std::vector<VkSemaphore> waitSemaphores = { window->GetSemaphore() };

        VkPipelineStageFlags pipelineStageFlags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        queue.Submit(&pipelineStageFlags, waitSemaphores, commandPool.GetVkCommandBuffers(commandBufferIndexes),
        {}, window->GetFence());

        window->EndRender({}, { window->GetFence() });
    }
    else
    {
        SetUpdateEnabled(false);
        SetPendingKill(true);
    }

    queue.WaitIdle();

    // Cleanup

}

Buffer& Renderer::GetUniformBuffer(uint32_t index)
{
    return uniformBuffers[index];
}

VertexBuffer& Renderer::GetVertexBuffer()
{
    return vertexBuffer;
}

void Renderer::BindVertexBuffers(VkCommandBuffer commandBuffer, std::vector<VkDeviceSize> offsets)
{
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffer.GetVkBufferPtr(), offsets.data());
}

void Renderer::CreateDescriptorSetLayout()
{
    VK_PERFORMANCE_SECTION("Create Descriptor Set Layout");

    std::vector<VkDescriptorSetLayoutBinding> layoutBindings;

    // Vertex uniform buffer.
    layoutBindings.push_back(
    {
        0,
        VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
        1,
        VK_SHADER_STAGE_VERTEX_BIT,
        nullptr
    });

    // Tessellation control shader.
    //layoutBindings.push_back(
    //{
    //    1,
    //    VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
    //    1,
    //    VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT,
    //    nullptr
    //});

    // Tessellation evaluation shader.
    //layoutBindings.push_back(
    //{
    //    2,
    //    VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
    //    1,
    //    VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT,
    //    nullptr
    //});

    //// Texture.
    //if (bTextureEnabled)
    //{
    //    layoutBindings.push_back(
    //    {
    //        1,
    //        VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
    //        1,
    //        VK_SHADER_STAGE_FRAGMENT_BIT,
    //        nullptr
    //    });
    //}

    descriptorSetLayoutCreateInfo = {
        VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        nullptr,
        0,
        (uint32_t)layoutBindings.size(),
        layoutBindings.data()
    };

    descriptorSetLayouts.resize(VULKAN_DESCRIPTOR_SETS_COUNT);
    VK_VERIFY(vkCreateDescriptorSetLayout(device->GetVkDevice(), &descriptorSetLayoutCreateInfo, nullptr, descriptorSetLayouts.data()));
}

void Renderer::InitDescriptorPool()
{
    descriptorPoolSizes.push_back({
        VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
        1
    });

    if (bTextureEnabled)
    {
        descriptorPoolSizes.push_back({
            VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            1
        });
    }

    descriptorPoolCreateInfo = {
        VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        nullptr,
        0,
        VULKAN_DESCRIPTOR_SETS_COUNT,
        (uint32_t)descriptorPoolSizes.size(),
        descriptorPoolSizes.data()
    };

    VK_VERIFY(vkCreateDescriptorPool(device->GetVkDevice(), &descriptorPoolCreateInfo, nullptr, &descriptorPool));
}

void Renderer::InitDescriptorSet()
{
    descriptorSetAllocateInfo = {
        VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        nullptr,
        descriptorPool,
        VULKAN_DESCRIPTOR_SETS_COUNT,
        descriptorSetLayouts.data()
    };

    descriptorSets.resize(VULKAN_DESCRIPTOR_SETS_COUNT);
    VK_VERIFY(vkAllocateDescriptorSets(device->GetVkDevice(), &descriptorSetAllocateInfo, descriptorSets.data()));

    for (uint32_t i = 0; i < VULKAN_DESCRIPTOR_SETS_COUNT; ++i)
    {
        writeDescriptorSets.push_back({
            VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            nullptr,
            descriptorSets[i],
            0,
            0,
            1,
            VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
            nullptr,
            uniformBuffers[i].GetDescriptorInfo(),
            nullptr
        });
    }

    vkUpdateDescriptorSets(device->GetVkDevice(), (uint32_t)writeDescriptorSets.size(), writeDescriptorSets.data(), 0, nullptr);
}

void Renderer::BindDescriptorSets(VkCommandBuffer commandBuffer, VkPipelineBindPoint pipelineBindPoint, std::vector<uint32_t> dynamicOffsets)
{
    vkCmdBindDescriptorSets(commandBuffer, pipelineBindPoint, pipelineLayout, 0,
        VULKAN_DESCRIPTOR_SETS_COUNT, descriptorSets.data(), (uint32_t)dynamicOffsets.size(), dynamicOffsets.data());
}

void Renderer::CreatePipelineLayout()
{
    VK_PERFORMANCE_SECTION("Create Pipeline Layout");

    pipelineLayoutCreateInfo = {
        VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        nullptr,
        0,
        (uint32_t)descriptorSetLayouts.size(),
        descriptorSetLayouts.data(),
        0,
        nullptr
    };

    VK_VERIFY(vkCreatePipelineLayout(device->GetVkDevice(), &pipelineLayoutCreateInfo, nullptr, &pipelineLayout));
}

void Renderer::InitPipelineCache()
{
    pipelineCacheCreateInfo = {
        VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO,
        nullptr,
        0,
        0,
        nullptr
    };

    VK_VERIFY(vkCreatePipelineCache(device->GetVkDevice(), &pipelineCacheCreateInfo, NULL, &pipelineCache));
}

void Renderer::InitPipeline(VkExtent2D size, VkRenderPass renderPass)
{
    VkDynamicState dynamicStates[VK_DYNAMIC_STATE_RANGE_SIZE];
    memset(dynamicStates, 0, sizeof(dynamicStates));

    dynamicStateCreateInfo = {
        VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        nullptr,
        0,
        0,
        dynamicStates
    };

    pipelineVertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    if (bIncludeVertexInput)
    {
        pipelineVertexInputStateCreateInfo = {
            VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
            nullptr,
            0,
            (uint32_t)vertexInputBindings.size(),
            vertexInputBindings.data(),
            (uint32_t)vertexInputAttributes.size(),
            vertexInputAttributes.data()
        };
    }

    pipelineInputAssemblyStateCreateInfo = {
        VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        nullptr,
        0,
        VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        VK_FALSE
    };

    pipelineRasterizationCreateInfo = {
        VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        nullptr,
        0,
        VK_FALSE,
        VK_FALSE,
        polygonMode,
        VK_CULL_MODE_BACK_BIT,
        VK_FRONT_FACE_CLOCKWISE,
        VK_FALSE,
        0,
        0,
        0,
        1.f
    };

    pipelineColorBlendAttachmentState.push_back({
        VK_FALSE,
        VK_BLEND_FACTOR_ZERO,
        VK_BLEND_FACTOR_ZERO,
        VK_BLEND_OP_ADD,
        VK_BLEND_FACTOR_ZERO,
        VK_BLEND_FACTOR_ZERO,
        VK_BLEND_OP_ADD,
        0xf
    });

    pipelineColorBlendStateCreateInfo = {
        VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        nullptr,
        0,
        VK_FALSE,
        VK_LOGIC_OP_NO_OP,
        (uint32_t)pipelineColorBlendAttachmentState.size(),
        pipelineColorBlendAttachmentState.data(),
        { 1.f, 1.f, 1.f, 1.f }
    };

    pipelineViewportStateCreateInfo = {
        VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        nullptr,
        0,
        VULKAN_VIEWPORT_COUNT,
        nullptr,
        VULKAN_SCISSOR_COUNT,
        nullptr
    };

    viewports.push_back({
        0.f, 0.f,
        (float)size.width / VULKAN_VIEWPORT_COUNT, (float)size.height,
        0.f, 1.f
    });

    scissors.push_back({
        { 0, 0 },
        { size.width / VULKAN_COMMAND_BUFFER_COUNT, size.height }
    });

#ifdef VULKAN_VR_MODE
    scissors.push_back({
        { (int32_t)size.width / VULKAN_COMMAND_BUFFER_COUNT, 0 },
        { size.width / VULKAN_COMMAND_BUFFER_COUNT, size.height }
    });
#elif defined(VULKAN_VR_MODE_VIEWPORTS)
    viewports.push_back({
        (float)size.width / VULKAN_VIEWPORT_COUNT, 0.f,
        (float)size.width / VULKAN_VIEWPORT_COUNT, (float)size.height,
        0.f, 1.f
    });

    scissors.push_back({
        { (int32_t)size.width / VULKAN_VIEWPORT_COUNT, 0 },
        { size.width / VULKAN_VIEWPORT_COUNT, size.height }
    });
#endif

    dynamicStates[dynamicStateCreateInfo.dynamicStateCount++] = VK_DYNAMIC_STATE_VIEWPORT;
    dynamicStates[dynamicStateCreateInfo.dynamicStateCount++] = VK_DYNAMIC_STATE_SCISSOR;

    VkStencilOpState stencilOpState = {
        VK_STENCIL_OP_KEEP,
        VK_STENCIL_OP_KEEP,
        VK_STENCIL_OP_KEEP,
        VK_COMPARE_OP_ALWAYS,
        0,
        0,
        0
    };

    pipelineDepthStencilStateCreateInfo = {
        VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
        nullptr,
        0,
        VK_TRUE,
        VK_TRUE,
        VK_COMPARE_OP_LESS_OR_EQUAL,
        VK_FALSE,
        VK_FALSE,
        stencilOpState,
        stencilOpState,
        0,
        0
    };

    pipelineMultisampleStateCreateInfo = {
        VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        nullptr,
        0,
        VULKAN_SAMPLE_COUNT,
        VK_FALSE,
        0.f,
        nullptr,
        VK_FALSE,
        VK_FALSE
    };

    graphicsPipelineCreateInfo = {
        VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        nullptr,
        0,
        (uint32_t)pipelineShaderStageCreateInfo.size(),
        pipelineShaderStageCreateInfo.data(),
        &pipelineVertexInputStateCreateInfo,
        &pipelineInputAssemblyStateCreateInfo,
        nullptr,
        &pipelineViewportStateCreateInfo,
        &pipelineRasterizationCreateInfo,
        &pipelineMultisampleStateCreateInfo,
        &pipelineDepthStencilStateCreateInfo,
        &pipelineColorBlendStateCreateInfo,
        &dynamicStateCreateInfo,
        pipelineLayout,
        renderPass,
        0,
        VK_NULL_HANDLE,
        0
    };

    VK_VERIFY(vkCreateGraphicsPipelines(device->GetVkDevice(), pipelineCache, 1, &graphicsPipelineCreateInfo, nullptr, &pipeline));
}

void Renderer::BindPipeline(VkCommandBuffer commandBuffer, VkPipelineBindPoint pipelineBindPoint)
{
    vkCmdBindPipeline(commandBuffer, pipelineBindPoint, pipeline);
}

void Renderer::AddVertexInputBinding(uint32_t binding, uint32_t stride, VkVertexInputRate inputRate)
{
    vertexInputBindings.push_back({
        binding, stride, inputRate
    });
}

void Renderer::AddVertexInputAttribute(uint32_t location, uint32_t binding, VkFormat format, uint32_t offset)
{
    vertexInputAttributes.push_back({
        location, binding, format, offset
    });
}

size_t Renderer::CompileShader(const char* shaderText, VkShaderStageFlagBits shaderType)
{
    VK_ASSERT(shaderText, "Invalid shader text!");

    // Check for existing compiled shader.
    size_t hash = std::hash<std::string>{}(shaderText);
    ShaderCache shaderCache{ hash };
    if (shaderCacheData.find(hash) == shaderCacheData.end())
    //auto iterator = std::find(shaderCacheData.begin(), shaderCacheData.end(), shaderCache);
    //if (iterator == shaderCacheData.end())
    {
        ShaderTools::InitGLSLang();

        ShaderCache shaderCache{ hash };
        std::vector<unsigned int> spirvData;

        VK_ASSERT(ShaderTools::glslToSPIRV(shaderType, shaderText, spirvData), "Cannot convert shader to SPIR-V.\n%s", shaderText);

        shaderCache.shaderType = shaderType;
        shaderCache.spirvData = spirvData;

        shaderCacheData.insert(std::make_pair(hash, shaderCache));

        ShaderTools::FinalizeGLSLang();
    }

    return hash;
}

//void Renderer::InitShaders(const char* vertexShaderText, const char* fragmentShaderText)
//{
//    VK_PERFORMANCE_SECTION("Initialize Shaders");
//
//    if (!(vertexShaderText || fragmentShaderText))
//    {
//        VK_LOG(LogRenderer, Error, "Invalid shaders!");
//        return;
//    }
//
//    ShaderTools::InitGLSLang();
//
//    VkShaderModuleCreateInfo moduleCreateInfo;
//
//    if (vertexShaderText)
//    {
//        std::vector<unsigned int> vertexSPIRV;
//
//        pipelineShaderStageCreateInfo.push_back({
//            VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
//            nullptr,
//            0,
//            VK_SHADER_STAGE_VERTEX_BIT,
//            VK_NULL_HANDLE,
//            "main",
//            nullptr
//            });
//
//        VK_ASSERT(ShaderTools::glslToSPIRV(VK_SHADER_STAGE_VERTEX_BIT, vertexShaderText, vertexSPIRV), "Cannot convert shader to SPIR-V.\n%s", vertexShaderText);
//
//        moduleCreateInfo = {
//            VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
//            nullptr,
//            0,
//            vertexSPIRV.size() * sizeof(unsigned int),
//            vertexSPIRV.data()
//        };
//
//        VK_VERIFY(vkCreateShaderModule(device->GetVkDevice(), &moduleCreateInfo, NULL, &pipelineShaderStageCreateInfo[0].module));
//    }
//
//    if (fragmentShaderText)
//    {
//        std::vector<unsigned int> fragmentSPIRV;
//
//        pipelineShaderStageCreateInfo.push_back({
//            VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
//            nullptr,
//            0,
//            VK_SHADER_STAGE_FRAGMENT_BIT,
//            VK_NULL_HANDLE,
//            "main",
//            nullptr
//            });
//
//        VK_ASSERT(ShaderTools::glslToSPIRV(VK_SHADER_STAGE_FRAGMENT_BIT, fragmentShaderText, fragmentSPIRV), "Cannot convert shader to SPIR-V.\n%s", fragmentShaderText);
//
//        moduleCreateInfo = {
//            VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
//            nullptr,
//            0,
//            fragmentSPIRV.size() * sizeof(unsigned int),
//            fragmentSPIRV.data()
//        };
//
//        VK_VERIFY(vkCreateShaderModule(device->GetVkDevice(), &moduleCreateInfo, NULL, &pipelineShaderStageCreateInfo[1].module));
//    }
//
//    ShaderTools::FinalizeGLSLang();
//}

void Renderer::InitShaders(ShaderIndexData& shaderIndexData)
{
    uint32_t index = 0;
    for (size_t hash : shaderIndexData)
    {
        VK_ASSERT(shaderCacheData.find(hash) != shaderCacheData.end(), "Cannot find shader id: %lu", hash);

        pipelineShaderStageCreateInfo.push_back({
            VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            nullptr,
            0,
            shaderCacheData.at(hash).shaderType,
            VK_NULL_HANDLE,
            "main",
            nullptr
        });

        //VK_ASSERT(ShaderTools::glslToSPIRV(VK_SHADER_STAGE_VERTEX_BIT, vertexShaderText, vertexSPIRV), "Cannot convert shader to SPIR-V.\n%s", vertexShaderText);

        VkShaderModuleCreateInfo moduleCreateInfo = {};

        moduleCreateInfo = {
            VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
            nullptr,
            0,
            shaderCacheData.at(hash).spirvData.size() * sizeof(unsigned int),
            shaderCacheData.at(hash).spirvData.data()
        };

        VK_VERIFY(vkCreateShaderModule(device->GetVkDevice(), &moduleCreateInfo, NULL, &pipelineShaderStageCreateInfo[index].module));

        ++index;
    }
}

void Renderer::CommandSetViewports(VkCommandBuffer commandBuffer)
{
    vkCmdSetViewport(commandBuffer, 0, (uint32_t)viewports.size(), viewports.data());
}

void Renderer::CommandSetScissors(VkCommandBuffer commandBuffer, uint32_t index)
{
    vkCmdSetScissor(commandBuffer, 0, 1, &scissors[index]);
}

VkRect2D Renderer::GetRenderArea(uint32_t viewportIndex)
{
    VkRect2D renderArea{};

    renderArea.offset.x = 0;
    renderArea.offset.y = 0;
    renderArea.extent = engine->GetWindow()->GetSurfaceSize();

#ifdef VULKAN_VR_MODE
    renderArea.offset.x = viewportIndex * engine->GetWindow()->GetSurfaceSize().width / 2;
    renderArea.extent.width /= 2;
#endif

    return renderArea;
}

std::vector<VkClearValue> Renderer::GetClearValues()
{
    std::vector<VkClearValue> clearValues = std::vector<VkClearValue>(2);

    clearValues[0].color.float32[0] = 0.f;
    clearValues[0].color.float32[1] = 0.f;
    clearValues[0].color.float32[2] = 0.f;
    clearValues[0].color.float32[3] = 0.f;

    clearValues[1].depthStencil.depth = 1.f;
    clearValues[1].depthStencil.stencil = 0;

    return clearValues;
}

RenderComponent* Renderer::AddRenderComponent(VertexData vertexData, ShaderEntry shaderEntry)
{
    RenderComponent* renderComponent = new RenderComponent;

    renderComponent->SetVertexData(vertexData);

    for (const char* vertexShader : shaderEntry.vertexShaders)
    {
        renderComponent->GetShaderIndexData().push_back(CompileShader(vertexShader, VK_SHADER_STAGE_VERTEX_BIT));
    }

    for (const char* fragmentShader : shaderEntry.fragmentShaders)
    {
        renderComponent->GetShaderIndexData().push_back(CompileShader(fragmentShader, VK_SHADER_STAGE_FRAGMENT_BIT));
    }

    renderComponents.push_back(std::unique_ptr<RenderComponent>());
    renderComponents.back().reset(renderComponent);

    //return renderComponents[renderComponents.size() - 1].get();

    return renderComponents.back().get();
}
