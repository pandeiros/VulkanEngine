/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2018 Pawel Kaczynski
 */

#include "TestApplication.h"

#include "Core.h"
#include "World.h"
#include "Rendering/Cube.h"
#include "Rendering/ShaderTools.h"
#include "Utils/FileManager.h"

VK_DECLARE_LOG_CATEGORY(LogTestApplication);

VULKAN_NS_USING;

TestApplication::TestApplication()
    : Application("Vulkan Engine App Test", 1, VK_MAKE_VERSION(1, 0, 2))
{}

TestApplication::~TestApplication()
{
    Queue& queue = instance->GetDevice()->GetQueueRef();
    queue.WaitIdle();
}

void TestApplication::Init()
{
    VK_PERFORMANCE_DATA("Application initialization");

    Application::Init();

    Device* device = instance->GetDevice();

#ifdef __ANDROID__
    engine->GetWorld()->SetCamera(CameraMode::VR, 90.f, instance->GetWindow()->GetAspectRatio(), 0.1f, 200.f);
#else
    engine->GetWorld()->SetCamera(CameraMode::DEFAULT, 90.f, instance->GetWindow()->GetAspectRatio(), 0.1f, 200.f);
#endif

    engine->GetWorld()->SetLensUndistortionCoefficients(glm::vec4(-0.35, -0.24, 0.0, 0.0));
    engine->GetRenderer()->Init();


    {
        VK_PERFORMANCE_SECTION("Scene initialization");

        RenderComponent* cubeRenderComponent = engine->GetRenderer()->AddRenderComponent(Cube::GetCubeVertexData(), Cube::GetCubeShaderEntry());

        std::srand((unsigned)std::time(nullptr));

        // Floor
        {
            int32_t cubes = 10;

            for (int32_t i = 0; i < cubes; ++i)
            {
                for (int32_t j = 0; j < cubes; ++j)
                {
                    float baseX = (i - cubes / 2) * 20.f;
                    float baseZ = (j - cubes / 2) * 20.f;

                    Actor* actor = new Actor;
                    SceneComponent* sceneComponent = new SceneComponent;
                    actor->SetSceneComponent(sceneComponent);

                    sceneComponent->SetRenderComponent(cubeRenderComponent);

                    float iMapped = Math::MapToRange((float)i, 0.f, float(cubes - 1), 0.f, 0.7f);
                    float jMapped = Math::MapToRange((float)j, 0.f, float(cubes - 1), 0.f, 0.7f);
                    float colorValue = (iMapped + jMapped) / 2.f;
                    sceneComponent->SetColor({ colorValue, /*Math::MapToRange(iMapped, 0.f, 1.f, 0.5f, 0.8f)*/ colorValue, colorValue });

                    actor->SetScale({ 10.f, 1.f, 10.f });
                    actor->SetPosition({ baseX, -2.f, baseZ });
                    engine->GetWorld()->AddActor(actor);
                }
            }
        }

        // Walls
        {
            uint32_t horizontalCubes = 5;
            uint32_t verticalCubes = 3;
            float radius = (float)(horizontalCubes + 1) / 2.f;

            std::vector<glm::vec3> positions;

            for (uint32_t i = 0; i < 4; ++i)
            {
                float baseX = (float)std::sin(i * PI / 2.f) * radius;
                float baseZ = (float)std::cos(i * PI / 2.f) * radius;

                for (uint32_t j = 0; j < horizontalCubes; ++j)
                {
                    int32_t column = j - horizontalCubes / 2;

                    for (uint32_t k = 0; k < verticalCubes; ++k)
                    {
                        glm::vec3 pos(baseX + (baseZ / radius) * column, k, baseZ + (baseX / radius) * column);
                        TestCube* actor = new TestCube;
                        SceneComponent* sceneComponent = new SceneComponent;
                        actor->SetSceneComponent(sceneComponent);

                        sceneComponent->SetRenderComponent(cubeRenderComponent);

                        float jMapped = Math::MapToRange((float)j, 0.f, float(horizontalCubes - 1), 0.f, 1.f);
                        float kMapped = Math::MapToRange((float)k, 0.f, float(verticalCubes - 1), 0.f, 1.f);
                        float xMapped = Math::MapToRange(baseX / radius, -1.f, 1.f, 0.f, 1.f);
                        float zMapped = Math::MapToRange(baseZ / radius, -1.f, 1.f, 0.f, 1.f);
                        float r = xMapped * (jMapped + kMapped) / 2.f;
                        float g = zMapped * (jMapped + kMapped) / 2.f;
                        float b = (1.f - (xMapped + zMapped) / 2.f) * (jMapped + kMapped) / 2.f;

                        sceneComponent->SetColor({ r, g, b });

                        actor->SetScale({ 0.5f, 0.5f, 0.5f });

                        actor->originPos = glm::vec3(std::rand() % 200 - 100, std::rand() % 5, std::rand() % 200 - 100);
                        actor->destPos = pos;
                        actor->SetPosition(actor->originPos);
                        actor->SetUpdateEnabled(true);
                        positions.push_back(pos);

                        using namespace std::placeholders;
                        engine->GetInputManager()->BindEvent<float>(InputCode::GVR_BUTTON_CLICK,
                            InputEventDelegate<float>(InputEvent::ON_PRESSED, std::bind(&TestCube::OnModeChange, actor, _1, _2, _3)));

                        engine->GetWorld()->AddActor(actor);
                    }
                }
            }
        }

        // OBJ Loading
        {
            VertexData vertices;
            std::vector<uint32_t> indices;
            std::vector<glm::vec3> normals;

            if (FileManager::LoadOBJWithNormals("teapot.obj", vertices, indices, normals))
            {
                RenderComponent* objRenderComponent = engine->GetRenderer()->AddRenderComponent(
                    vertices, Cube::GetCubeShaderEntry());
                Actor* actor = new Actor;
                TestSceneComponent* sceneComponent = new TestSceneComponent;
                actor->SetSceneComponent(sceneComponent);
                sceneComponent->SetRenderComponent(objRenderComponent);

                engine->GetWorld()->AddActor(actor);

                actor->GetSceneComponent()->SetColor({ 1.f, 0.1f, 0.1f });
                //actor->SetTransform({glm::vec3(0.f), glm::vec3(5.f), glm::vec3(glm::radians(-90.f), glm::radians(90.f), 0.f)}); // chalet.obj
                //actor->SetTransform({glm::vec3(0.f, -1.f, 0.f), glm::vec3(0.05f), glm::vec3(glm::radians(-90.f), glm::radians(-75.f), 0.f)}); // minicooper.obj
                actor->SetTransform({ glm::vec3(0.f), glm::vec3(0.02f), glm::vec3(0.f) }); // teapot.obj
                actor->SetUpdateEnabled(false);
                //sceneComponent->UpdateData();

                RenderComponent* renderComponent = actor->GetSceneComponent()->GetRenderComponent();
                ShadeFaces(renderComponent, normals);
            }

            // Sun
            {
                if (FileManager::LoadOBJWithNormals("sphere.obj", vertices, indices, normals))
                {
                    RenderComponent* objRenderComponent = engine->GetRenderer()->AddRenderComponent(vertices, Cube::GetCubeShaderEntry());
                    Actor* actor = new Actor;
                    TestSceneComponent* sceneComponent = new TestSceneComponent;
                    actor->SetSceneComponent(sceneComponent);
                    sceneComponent->SetRenderComponent(objRenderComponent);

                    engine->GetWorld()->AddActor(actor);

                    actor->SetTransform({ glm::vec3(50.f, 50.f, -50.f), glm::vec3(10.f), glm::vec3(0.f) });
                    actor->GetSceneComponent()->SetColor({ 1.f, 1.f, 1.f });
                    actor->SetUpdateEnabled(false);

                    RenderComponent* renderComponent = actor->GetSceneComponent()->GetRenderComponent();
                    ColorSun(renderComponent, normals);
                }
            }

            // Clouds
            {
                uint32_t cloudCount = 5;
                std::string names[4] = {"cloud1.obj", "cloud2.obj", "cloud3.obj", "cloud4.obj"};
                for (uint32_t i = 0; i < cloudCount; ++i)
                {
                    for (uint32_t j = 0; j < 4; ++j)
                    {
                        std::string filename = names[j];
                        if (FileManager::LoadOBJWithNormals(filename, vertices, indices, normals))
                        {
                            RenderComponent* objRenderComponent = engine->GetRenderer()->AddRenderComponent(vertices, Cube::GetCubeShaderEntry());
                            TestCloud* actor = new TestCloud;
                            TestSceneComponent* sceneComponent = new TestSceneComponent;
                            actor->SetSceneComponent(sceneComponent);
                            sceneComponent->SetRenderComponent(objRenderComponent);

                            engine->GetWorld()->AddActor(actor);

                            glm::vec3 pos = glm::vec3(std::rand() % 200 - 100, 30.f, std::rand() % 200 - 100);
                            actor->SetPosition(pos);
                            actor->SetScale(glm::vec3(5.f));
                            actor->GetSceneComponent()->SetColor({ 1.f, 1.f, 1.f });
                            actor->SetUpdateEnabled(true);

                            RenderComponent* renderComponent = actor->GetSceneComponent()->GetRenderComponent();
                            ShadeFaces(renderComponent, normals);
                        }
                    }
                }
            }


            // Trees
            {
                uint32_t treesCount = 100;
                for (uint32_t i = 0; i < treesCount; ++i)
                {
                    for (uint32_t j = 0; j < 2; ++j)
                    {
                        std::string filename = j == 0 ? "lowpolytree1.obj" : "lowpolytree2.obj";
                        if (FileManager::LoadOBJWithNormals(filename, vertices, indices, normals))
                        {
                            RenderComponent* objRenderComponent = engine->GetRenderer()->AddRenderComponent(vertices, Cube::GetCubeShaderEntry());
                            Actor* actor = new Actor;
                            TestSceneComponent* sceneComponent = new TestSceneComponent;
                            actor->SetSceneComponent(sceneComponent);
                            sceneComponent->SetRenderComponent(objRenderComponent);

                            engine->GetWorld()->AddActor(actor);

                            float yPos = filename == "lowpolytree2.obj" ? -1.f : 1.f;
                            glm::vec3 pos = glm::vec3(std::rand() % 200 - 100, yPos, std::rand() % 200 - 100);
                            if (std::abs(pos.x) < 5.f)
                            {
                                pos.x += 5.f * (pos.x < 0.f ? -1.f : 1.f);
                            }
                            if (std::abs(pos.z) < 5.f)
                            {
                                pos.z += 5.f * (pos.z < 0.f ? -1.f : 1.f);
                            }

                            actor->SetPosition(pos);
                            actor->GetSceneComponent()->SetColor({ 0.2, 0.8f, 0.1f });
                            actor->SetUpdateEnabled(false);

                            if (filename == "lowpolytree2.obj")
                            {
                                actor->SetScale(glm::vec3(0.015f));
                            }

                            RenderComponent* renderComponent = actor->GetSceneComponent()->GetRenderComponent();
                            ColorTreeVertices(renderComponent, filename);
                            ShadeFaces(renderComponent, normals);
                        }
                    }
                }
            }
        }


        // Cubes
        {
            RenderComponent* renderComponent = engine->GetRenderer()->AddRenderComponent(Cube::GetCubeVertexData(), Cube::GetCubeShaderEntry());

            int32_t cubeCount = 0;

            for (int32_t i = 0; i < cubeCount; ++i)
            {
                for (int32_t j = 0; j < cubeCount; ++j)
                {
                    float xArg = float(i - cubeCount / 2);
                    float yArg = float(j - cubeCount / 2);
                    float y = 10 * ((float)std::cos(xArg / cubeCount * PI * 2) + (float)std::sin(yArg * 3 / 2 / cubeCount * PI * 2));

                    Actor* actor = new Actor;
                    SceneComponent* sceneComponent = new SceneComponent;
                    actor->SetSceneComponent(sceneComponent);
                    sceneComponent->SetRenderComponent(renderComponent);

                    actor->SetTransform({ glm::vec3(xArg * 0.75f, y, yArg * 0.75f), glm::vec3(0.25f, 0.25f, 0.25f) });
                    actor->SetPosition({ -1.f, 0.f, -1.f });
                    actor->SetScale({ 1.f, 1.f, 1.f });
                    actor->GetSceneComponent()->SetColor({ xArg / cubeCount + 0.5f, 0, yArg / cubeCount + 0.5f });
                    actor->GetSceneComponent()->SetColor({ 1.f, 1.f, 1.f });

                    engine->GetWorld()->AddActor(actor);
                }
            }
        }
    }

    timer = std::chrono::steady_clock();
    lastTime = timer.now();

    SetUpdateEnabled(true);

    VK_LOG(LogTestApplication, Info, "VULKAN APP INITIALIZED");
}

void TestApplication::Tick(float deltaTime)
{
    std::chrono::duration<double> diff = std::chrono::duration_cast<std::chrono::duration<double>>(timer.now() - lastTime);
    if (diff.count() >= 1.0)
    {
        lastTime = timer.now();
        VK_LOG(LogTestApplication, Debug, "FPS: %.0f", GetEngine()->GetFPS());
        //GetEngine()->RequestPerformanceDataLog();
    }

    return;
}

Actor* TestApplication::CreateActor(std::string filename)
{
    VertexData vertices;
    std::vector<uint32_t> indices;

    if (FileManager::LoadOBJ(filename, vertices, indices))
    {
        RenderComponent* objRenderComponent = engine->GetRenderer()->AddRenderComponent(vertices, Cube::GetCubeShaderEntry());
        Actor* actor = new Actor;
        TestSceneComponent* sceneComponent = new TestSceneComponent;
        actor->SetSceneComponent(sceneComponent);
        sceneComponent->SetRenderComponent(objRenderComponent);

        engine->GetWorld()->AddActor(actor);

        return actor;
    }

    return nullptr;
}

void TestApplication::ColorTreeVertices(RenderComponent* renderComponent, std::string filename)
{
    uint32_t size, stride;
    void* data = renderComponent->GetData(size, stride);
    Vertex* originVertexData = static_cast<Vertex*>(data);
    uint32_t vertexCount = size / sizeof(Vertex);

    for (uint32_t i = 0; i < vertexCount; ++i)
    {
        Vertex vertex = *(originVertexData + i);
        glm::vec3 brownColor = glm::vec3(0.5f, 0.25f, 0.f);
        glm::vec3 greenColor = glm::vec3((float)(std::rand() % 50) / 400.f, 0.5f - (float)(std::rand() % 50) / 400.f, 0.f);

        if ((filename == "lowpolytree1.obj" && vertex.y < -1.2f) || (filename == "lowpolytree2.obj" && vertex.y < 155.f))
        {
            (originVertexData + i)->r = brownColor.x;
            (originVertexData + i)->g = brownColor.y;
            (originVertexData + i)->b = brownColor.z;
        }
        else
        {
            (originVertexData + i)->r = greenColor.x;
            (originVertexData + i)->g = greenColor.y;
            (originVertexData + i)->b = greenColor.z;
        }
    }
}

void TestApplication::ColorSun(RenderComponent* renderComponent, std::vector<glm::vec3>& normals)
{
    uint32_t size, stride;
    void* data = renderComponent->GetData(size, stride);
    Vertex* originVertexData = static_cast<Vertex*>(data);
    uint32_t vertexCount = size / sizeof(Vertex);

    for (uint32_t i = 0; i < vertexCount; ++i)
    {
        float angle = glm::orientedAngle(normals[i], glm::vec3(-1.f, -1.f, 1.f), glm::vec3(-1.f, 1.f, 1.f));
        float angleParam = 1.f - std::abs(angle) / PI / 8.f;
        glm::vec3 yellowColor = glm::vec3(0.98f, 0.98f, 0.50f);

        (originVertexData + i)->r = yellowColor.x * angleParam;
        (originVertexData + i)->g = yellowColor.y * angleParam;
        (originVertexData + i)->b = yellowColor.z * angleParam;
    }
}

void TestApplication::ShadeFaces(RenderComponent* renderComponent, std::vector<glm::vec3>& normals, float factor)
{
    uint32_t size, stride;
    void* data = renderComponent->GetData(size, stride);
    Vertex* originVertexData = static_cast<Vertex*>(data);
    uint32_t vertexCount = size / sizeof(Vertex);

    for (uint32_t i = 0; i < vertexCount; ++i)
    {
        glm::vec3 pos = glm::vec3((originVertexData + i)->x, (originVertexData + i)->y, (originVertexData + i)->z);
        glm::vec3 sunPos = glm::vec3(50.f, 50.f, -50.f);

        float angle = glm::orientedAngle(normals[i], pos - sunPos, glm::vec3(-1.f, 1.f, 1.f));
        float angleParam = 1.f - std::abs(angle) / PI / factor;

        (originVertexData + i)->r = (originVertexData + i)->r * angleParam;
        (originVertexData + i)->g = (originVertexData + i)->g * angleParam;
        (originVertexData + i)->b = (originVertexData + i)->b * angleParam;
    }
}

void TestCube::Tick(float deltaTime)
{
    if (timer == 0.f)
    {
        int rand = std::rand() % 1000;
        timer = float(rand) / 100.f;
    }

    timer += deltaTime;

    glm::vec3 deltaPos = (bAnimSwitch ? destPos : originPos) - transform.position;
    transform.position += deltaPos * deltaTime;

    if (!bAnimSwitch)
    {
        transform.position.y += std::sin(timer / 2.f) * 0.01f;
    }

    sceneComponent->UpdateData();
}

void TestCube::OnModeChange(InputCode inputCode, InputEvent event, float value)
{
    bAnimSwitch = !bAnimSwitch;
}

void TestSceneComponent::UpdateData()
{
    if (!vertexData)
    {
        return;
    }

    uint32_t size, stride;
    void* data = renderComponent->GetData(size, stride);
    Vertex* originVertexData = static_cast<Vertex*>(data);

    uint32_t vertexCount = size / sizeof(Vertex);
    Transform transform = GetTransform();

    for (uint32_t i = 0; i < vertexCount; ++i)
    {
        (vertexData + i)->x = (originVertexData + i)->x * transform.scale.x;
        (vertexData + i)->y = (originVertexData + i)->y * transform.scale.y;
        (vertexData + i)->z = (originVertexData + i)->z * transform.scale.z;

        if (!bRotationSet)
        {
            glm::quat quatRotation = glm::quat(transform.rotation);
            glm::vec3 vertex = glm::vec3((vertexData + i)->x, (vertexData + i)->y, (vertexData + i)->z);
            vertex = quatRotation * vertex;
            (vertexData + i)->x = vertex.x;
            (vertexData + i)->y = vertex.y;
            (vertexData + i)->z = vertex.z;
        }

        (vertexData + i)->x += transform.position.x;
        (vertexData + i)->y += transform.position.y;
        (vertexData + i)->z += transform.position.z;
    }

    bRotationSet = true;
}

void TestSceneComponent::SetColor(const glm::vec3 color)
{
    uint32_t size, stride;
    void* data = renderComponent->GetData(size, stride);
    Vertex* originVertexData = static_cast<Vertex*>(data);
    uint32_t vertexCount = size / sizeof(Vertex);

    for (uint32_t i = 0; i < vertexCount; ++i)
    {
        (originVertexData + i)->r = color.x;
        (originVertexData + i)->g = color.y;
        (originVertexData + i)->b = color.z;
    }

}

void TestCloud::Tick(float deltaTime)
{
    if (speed == 0.f)
    {
        speed = 0.5f + float(std::rand() % 100) / 300.f;
    }

    glm::vec3 deltaPos = glm::vec3(1.f, 0.f, 0.3f) * speed * deltaTime;
    transform.position += deltaPos;

    if (transform.position.x > 100)
    {
        transform.position.x = -100;
    }
    else if (transform.position.z > 100)
    {
        transform.position.z = -100;
    }


    sceneComponent->UpdateData();
}
