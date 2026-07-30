#include "EnergyBeamComponent.h"

#include <RTBEngine/Core/Logger.h>
#include <RTBEngine/Core/ResourceManager.h>
#include <RTBEngine/Core/Time.h>
#include <RTBEngine/Rendering/Camera.h>
#include <RTBEngine/Rendering/CameraUBO.h>
#include <RTBEngine/Rendering/Shader.h>
#include <RTBEngine/Rendering/RHI/RenderDevice.h>
#include <RTBEngine/Scene/GameObject.h>

#include <algorithm>
#include <cmath>

namespace {
    constexpr float kMinRadius = 0.01f;
    constexpr float kSegmentEpsilon = 0.000001f;
    constexpr int kBeamShaderVersion = 2;
    int g_loadedBeamShaderVersion = 0;
}

using ThisClass = EnergyBeamComponent;
RTB_REGISTER_COMPONENT(EnergyBeamComponent)
    RTB_PROPERTY(playing)
    RTB_PROPERTY(visible)
    RTB_PROPERTY(simulateInEditMode)
    RTB_PROPERTY(useVerticalCross)
    RTB_PROPERTY_RANGE(length, 0.1f, 40.0f)
    RTB_PROPERTY_RANGE(radius, 0.01f, 8.0f)
    RTB_PROPERTY_RANGE(duration, 0.0f, 60.0f)
    RTB_PROPERTY_RANGE(coreWidthScale, 0.05f, 2.0f)
    RTB_PROPERTY_RANGE(innerWidthScale, 0.1f, 3.0f)
    RTB_PROPERTY_RANGE(outerWidthScale, 0.1f, 4.0f)
    RTB_PROPERTY_COLOR(beamColor)
    RTB_PROPERTY_COLOR(coreColor)
    RTB_PROPERTY_RANGE(emissionStrength, 0.0f, 16.0f)
    RTB_PROPERTY_RANGE(noiseScale, 0.1f, 20.0f)
    RTB_PROPERTY_RANGE(noiseSpeed, 0.0f, 20.0f)
    RTB_PROPERTY_RANGE(distortionStrength, 0.0f, 2.0f)
    RTB_PROPERTY_RANGE(fresnelPower, 0.1f, 8.0f)
    RTB_PROPERTY_RANGE(glowIntensity, 0.0f, 8.0f)
    RTB_PROPERTY_RANGE(pulseSpeed, 0.0f, 20.0f)
    RTB_PROPERTY_RANGE(pulseAmount, 0.0f, 1.0f)
    RTB_PROPERTY_RANGE(softEdge, 0.05f, 4.0f)
    RTB_PROPERTY_RANGE(uvScrollSpeed, -20.0f, 20.0f)
    RTB_PROPERTY_RANGE(uvTilesPerMeter, 0.0f, 10.0f)
    RTB_PROPERTY(shaderRef)
    RTB_PROPERTY(vertexPath)
    RTB_PROPERTY(fragmentPath)
RTB_END_REGISTER(EnergyBeamComponent)

EnergyBeamComponent::EnergyBeamComponent()
    : Component()
{
}

EnergyBeamComponent::~EnergyBeamComponent()
{
    ReleaseRenderResources();
}

void EnergyBeamComponent::OnValidate()
{
    length = std::max(0.1f, length);
    radius = std::max(kMinRadius, radius);
    duration = std::max(0.0f, duration);
    coreWidthScale = std::max(0.05f, coreWidthScale);
    innerWidthScale = std::max(0.1f, innerWidthScale);
    outerWidthScale = std::max(0.1f, outerWidthScale);
    softEdge = std::max(0.05f, softEdge);
    uvTilesPerMeter = std::max(0.0f, uvTilesPerMeter);
    emissionStrength = std::max(0.0f, emissionStrength);
    glowIntensity = std::max(0.0f, glowIntensity);
}

void EnergyBeamComponent::OnDestroy()
{
    ReleaseRenderResources();
}

void EnergyBeamComponent::OnUpdate(float deltaTime)
{
    if (!playing) {
        return;
    }

    animTime += deltaTime;
    elapsed += deltaTime;
    if (duration > 0.0f && elapsed >= duration) {
        Stop();
    }
}

void EnergyBeamComponent::OnEditModeSimulate(float deltaTime)
{
    if (!simulateInEditMode || !visible) {
        return;
    }
    animTime += deltaTime;
}

void EnergyBeamComponent::OnTransparentRender(RTBEngine::Rendering::Camera* camera)
{
    Render(camera);
}

void EnergyBeamComponent::SetEndpoints(
    const RTBEngine::Math::Vector3& start,
    const RTBEngine::Math::Vector3& end)
{
    points.clear();
    points.push_back(start);
    points.push_back(end);
    length = std::max((end - start).Length(), 0.1f);
}

void EnergyBeamComponent::SetBeam(
    const RTBEngine::Math::Vector3& origin,
    const RTBEngine::Math::Vector3& direction,
    float beamLength)
{
    RTBEngine::Math::Vector3 dir = direction;
    if (dir.LengthSquared() <= kSegmentEpsilon) {
        ClearBeam();
        return;
    }
    dir.Normalize();
    length = std::max(beamLength, 0.1f);
    points.clear();
    points.push_back(origin);
    points.push_back(origin + dir * length);
}

void EnergyBeamComponent::ClearBeam()
{
    points.clear();
}

void EnergyBeamComponent::Play()
{
    playing = true;
    elapsed = 0.0f;
    SetUpdateTickEnabled(true);
}

void EnergyBeamComponent::Stop()
{
    playing = false;
    ClearBeam();
    SetUpdateTickEnabled(false);
}

void EnergyBeamComponent::ApplyDefaults()
{
    length = 8.0f;
    radius = 0.90f;
    duration = 0.0f;
    useVerticalCross = true;
    coreWidthScale = 0.55f;
    innerWidthScale = 1.00f;
    outerWidthScale = 1.65f;
    beamColor = RTBEngine::Math::Vector4(0.35f, 0.85f, 1.00f, 0.85f);
    coreColor = RTBEngine::Math::Vector4(1.00f, 0.98f, 0.94f, 1.00f);
    emissionStrength = 2.5f;
    noiseScale = 3.5f;
    noiseSpeed = 2.2f;
    distortionStrength = 0.15f;
    fresnelPower = 2.5f;
    glowIntensity = 1.4f;
    pulseSpeed = 4.0f;
    pulseAmount = 0.18f;
    softEdge = 0.85f;
    uvScrollSpeed = 2.5f;
    uvTilesPerMeter = 0.35f;
}

bool EnergyBeamComponent::EnsureRenderResources()
{
    if (vao != RTBEngine::Rendering::RHI::kInvalidGpuId &&
        vbo != RTBEngine::Rendering::RHI::kInvalidGpuId &&
        shader &&
        g_loadedBeamShaderVersion == kBeamShaderVersion) {
        return true;
    }

    if (vao != RTBEngine::Rendering::RHI::kInvalidGpuId ||
        vbo != RTBEngine::Rendering::RHI::kInvalidGpuId) {
        ReleaseRenderResources();
    }

    const std::string shaderName = shaderRef.empty() ? "energy_beam" : shaderRef;
    const std::string vertPath = vertexPath.empty() ? "Assets/Shaders/EnergyBeam.vert" : vertexPath;
    const std::string fragPath = fragmentPath.empty() ? "Assets/Shaders/EnergyBeam.frag" : fragmentPath;
    if (!shader) {
        shader = RTBEngine::Core::ResourceManager::GetInstance().GetShader(shaderName);
        if (!shader) {
            shader = RTBEngine::Core::ResourceManager::GetInstance().LoadShader(
                shaderName,
                vertPath,
                fragPath);
        }
    }

    if (!shader) {
        RTB_ERROR("EnergyBeamComponent: failed to load energy_beam shader.");
        return false;
    }

    if (g_loadedBeamShaderVersion != kBeamShaderVersion) {
        const std::string resolvedVert =
            RTBEngine::Core::ResourceManager::GetInstance().ResolvePathForRead(vertPath);
        const std::string resolvedFrag =
            RTBEngine::Core::ResourceManager::GetInstance().ResolvePathForRead(fragPath);
        if (!shader->LoadFromFiles(resolvedVert, resolvedFrag)) {
            RTB_ERROR("EnergyBeamComponent: failed to reload energy_beam shader.");
            shader = nullptr;
            return false;
        }
        g_loadedBeamShaderVersion = kBeamShaderVersion;
    }

    auto& device = RTBEngine::Rendering::RHI::RenderDevice::Get();
    vao = device.CreateVertexArray();
    vbo = device.CreateBuffer();

    device.BindVertexArray(vao);
    device.SetArrayBufferData(
        vbo,
        nullptr,
        sizeof(BeamVertex),
        RTBEngine::Rendering::RHI::BufferUsage::Dynamic);

    device.EnableVertexAttribFloat(0, 3, static_cast<int>(sizeof(BeamVertex)), offsetof(BeamVertex, position));
    device.EnableVertexAttribFloat(1, 4, static_cast<int>(sizeof(BeamVertex)), offsetof(BeamVertex, color));
    device.EnableVertexAttribFloat(2, 2, static_cast<int>(sizeof(BeamVertex)), offsetof(BeamVertex, uv));
    device.EnableVertexAttribFloat(3, 1, static_cast<int>(sizeof(BeamVertex)), offsetof(BeamVertex, side));
    device.UnbindVertexArray();
    return true;
}

void EnergyBeamComponent::ReleaseRenderResources()
{
    if (!RTBEngine::Rendering::RHI::RenderDevice::HasDevice()) {
        vao = RTBEngine::Rendering::RHI::kInvalidGpuId;
        vbo = RTBEngine::Rendering::RHI::kInvalidGpuId;
        shader = nullptr;
        return;
    }

    auto& device = RTBEngine::Rendering::RHI::RenderDevice::Get();
    if (vao != RTBEngine::Rendering::RHI::kInvalidGpuId) {
        device.DestroyVertexArray(vao);
        vao = RTBEngine::Rendering::RHI::kInvalidGpuId;
    }
    if (vbo != RTBEngine::Rendering::RHI::kInvalidGpuId) {
        device.DestroyBuffer(vbo);
        vbo = RTBEngine::Rendering::RHI::kInvalidGpuId;
    }
    shader = nullptr;
}

void EnergyBeamComponent::AppendRibbon(
    std::vector<BeamVertex>& outVertices,
    RTBEngine::Rendering::Camera* camera,
    float width,
    const RTBEngine::Math::Vector4& color,
    bool vertical) const
{
    if (!camera || points.size() < 2 || width <= kMinRadius) {
        return;
    }

    const RTBEngine::Math::Vector3 up = RTBEngine::Math::Vector3::Up();
    const float halfWidth = width * 0.5f;
    const float scrollOffset = animTime * uvScrollSpeed;

    std::vector<float> cumulativeDistance(points.size(), 0.0f);
    for (std::size_t i = 1; i < points.size(); ++i) {
        cumulativeDistance[i] = cumulativeDistance[i - 1] + (points[i] - points[i - 1]).Length();
    }

    auto pushVertex = [&](const RTBEngine::Math::Vector3& position,
                          const RTBEngine::Math::Vector4& vertexColor,
                          float u,
                          float side) {
        BeamVertex vertex{};
        vertex.position[0] = position.x;
        vertex.position[1] = position.y;
        vertex.position[2] = position.z;
        vertex.color[0] = vertexColor.x;
        vertex.color[1] = vertexColor.y;
        vertex.color[2] = vertexColor.z;
        vertex.color[3] = vertexColor.w;
        vertex.uv[0] = u + scrollOffset;
        vertex.uv[1] = side * 0.5f + 0.5f;
        vertex.side = side;
        outVertices.push_back(vertex);
    };

    for (std::size_t i = 0; i + 1 < points.size(); ++i) {
        const RTBEngine::Math::Vector3 start = points[i];
        const RTBEngine::Math::Vector3 end = points[i + 1];

        RTBEngine::Math::Vector3 direction = end - start;
        if (!vertical) {
            direction.y = 0.0f;
        }
        if (direction.LengthSquared() <= kSegmentEpsilon) {
            continue;
        }
        direction.Normalize();

        RTBEngine::Math::Vector3 sideDirection;
        if (vertical) {
            sideDirection = up;
            sideDirection = sideDirection - direction * sideDirection.Dot(direction);
            if (sideDirection.LengthSquared() <= kSegmentEpsilon) {
                sideDirection = camera->GetUp();
                sideDirection = sideDirection - direction * sideDirection.Dot(direction);
            }
        } else {
            sideDirection = up.Cross(direction);
        }

        if (sideDirection.LengthSquared() <= kSegmentEpsilon) {
            continue;
        }
        sideDirection.Normalize();

        const RTBEngine::Math::Vector3 side = sideDirection * halfWidth;
        const RTBEngine::Math::Vector3 p0 = start - side;
        const RTBEngine::Math::Vector3 p1 = start + side;
        const RTBEngine::Math::Vector3 p2 = end + side;
        const RTBEngine::Math::Vector3 p3 = end - side;

        const float startU = cumulativeDistance[i] * uvTilesPerMeter;
        const float endU = cumulativeDistance[i + 1] * uvTilesPerMeter;

        pushVertex(p0, color, startU, -1.0f);
        pushVertex(p1, color, startU, 1.0f);
        pushVertex(p2, color, endU, 1.0f);
        pushVertex(p0, color, startU, -1.0f);
        pushVertex(p2, color, endU, 1.0f);
        pushVertex(p3, color, endU, -1.0f);
    }
}

void EnergyBeamComponent::Render(RTBEngine::Rendering::Camera* camera)
{
    if (!isEnabled || !visible || !camera || points.size() < 2) {
        return;
    }
    if (!GetOwner() || !GetOwner()->IsActiveInHierarchy()) {
        return;
    }
    if (!EnsureRenderResources()) {
        return;
    }

    std::vector<BeamVertex> vertices;
    vertices.reserve(points.size() * 18);

    const float base = std::max(radius, kMinRadius);
    AppendRibbon(vertices, camera, base * outerWidthScale, beamColor, false);
    AppendRibbon(vertices, camera, base * innerWidthScale, beamColor, false);
    AppendRibbon(vertices, camera, base * coreWidthScale, coreColor, false);

    if (useVerticalCross) {
        AppendRibbon(vertices, camera, base * outerWidthScale * 0.85f, beamColor, true);
        AppendRibbon(vertices, camera, base * innerWidthScale * 0.85f, beamColor, true);
        AppendRibbon(vertices, camera, base * coreWidthScale * 0.90f, coreColor, true);
    }

    if (vertices.empty()) {
        return;
    }

    auto& device = RTBEngine::Rendering::RHI::RenderDevice::Get();
    device.SetArrayBufferData(
        vbo,
        vertices.data(),
        vertices.size() * sizeof(BeamVertex),
        RTBEngine::Rendering::RHI::BufferUsage::Dynamic);

    device.SetDepthTest(true);
    device.SetDepthFunc(RTBEngine::Rendering::RHI::DepthFunc::LEqual);
    device.SetDepthWrite(false);
    device.SetBlend(true);
    device.SetBlendFuncSeparate(
        RTBEngine::Rendering::RHI::BlendFactor::SrcAlpha,
        RTBEngine::Rendering::RHI::BlendFactor::One,
        RTBEngine::Rendering::RHI::BlendFactor::SrcAlpha,
        RTBEngine::Rendering::RHI::BlendFactor::One);
    device.SetCullFace(false);

    shader->Bind();
    RTBEngine::Rendering::CameraUBO::GetInstance().Bind();
    shader->SetVector4("uBeamColor", beamColor);
    shader->SetVector4("uCoreColor", coreColor);
    shader->SetFloat("uEmissionStrength", emissionStrength);
    shader->SetFloat("uNoiseScale", noiseScale);
    shader->SetFloat("uNoiseSpeed", noiseSpeed);
    shader->SetFloat("uDistortionStrength", distortionStrength);
    shader->SetFloat("uFresnelPower", fresnelPower);
    shader->SetFloat("uGlowIntensity", glowIntensity);
    shader->SetFloat("uPulseSpeed", pulseSpeed);
    shader->SetFloat("uPulseAmount", pulseAmount);
    shader->SetFloat("uSoftEdge", softEdge);
    shader->SetFloat("uTime", RTBEngine::Core::Time::GetTime());
    shader->SetVector3("uViewPos", camera->GetPosition());

    device.BindVertexArray(vao);
    device.DrawArrays(
        RTBEngine::Rendering::RHI::PrimitiveTopology::Triangles,
        0,
        static_cast<int>(vertices.size()));
    device.UnbindVertexArray();
    shader->Unbind();

    device.SetDepthWrite(true);
    device.SetBlend(false);
    device.SetCullFace(true);
    device.SetDepthFunc(RTBEngine::Rendering::RHI::DepthFunc::Less);
    device.SetDepthTest(true);
}
