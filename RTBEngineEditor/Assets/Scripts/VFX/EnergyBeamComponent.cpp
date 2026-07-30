#include "EnergyBeamComponent.h"

#include <RTBEngine/Core/Logger.h>
#include <RTBEngine/Core/ResourceManager.h>
#include <RTBEngine/Core/Time.h>
#include <RTBEngine/Math/Quaternions/Quaternion.h>
#include <RTBEngine/Rendering/Camera.h>
#include <RTBEngine/Rendering/CameraUBO.h>
#include <RTBEngine/Rendering/Shader.h>
#include <RTBEngine/Rendering/RHI/RenderDevice.h>
#include <RTBEngine/Scene/GameObject.h>
#include <RTBEngine/Scene/ParticleSystem.h>

#include <algorithm>
#include <cmath>

namespace {
    constexpr float kMinRadius = 0.01f;
    constexpr float kSegmentEpsilon = 0.000001f;
    constexpr float kPi = 3.14159265358979323846f;
    constexpr int kBeamShaderVersion = 5;
    int g_loadedBeamShaderVersion = 0;

    bool BuildFrame(
        const RTBEngine::Math::Vector3& axisIn,
        RTBEngine::Math::Vector3& outAxis,
        RTBEngine::Math::Vector3& outRight,
        RTBEngine::Math::Vector3& outUp)
    {
        outAxis = axisIn;
        if (outAxis.LengthSquared() <= kSegmentEpsilon) {
            return false;
        }
        outAxis.Normalize();

        outRight = RTBEngine::Math::Vector3::Up().Cross(outAxis);
        if (outRight.LengthSquared() <= kSegmentEpsilon) {
            outRight = RTBEngine::Math::Vector3::Right().Cross(outAxis);
        }
        if (outRight.LengthSquared() <= kSegmentEpsilon) {
            return false;
        }
        outRight.Normalize();
        outUp = outAxis.Cross(outRight);
        outUp.Normalize();
        return true;
    }
}

using ThisClass = EnergyBeamComponent;
RTB_REGISTER_COMPONENT(EnergyBeamComponent)
    RTB_PROPERTY(playing)
    RTB_PROPERTY(visible)
    RTB_PROPERTY(simulateInEditMode)
    RTB_PROPERTY(useVerticalCross)
    RTB_PROPERTY_RANGE(radialSegments, 6, 32)
    RTB_PROPERTY_RANGE(length, 0.1f, 40.0f)
    RTB_PROPERTY_RANGE(radius, 0.01f, 8.0f)
    RTB_PROPERTY_RANGE(duration, 0.0f, 60.0f)
    RTB_PROPERTY_RANGE(coreWidthScale, 0.05f, 2.0f)
    RTB_PROPERTY_RANGE(innerWidthScale, 0.1f, 3.0f)
    RTB_PROPERTY_RANGE(outerWidthScale, 0.1f, 4.0f)
    RTB_PROPERTY_RANGE(taperAmount, 0.0f, 0.95f)
    RTB_PROPERTY_RANGE(tipCapScale, 0.1f, 4.0f)
    RTB_PROPERTY_RANGE(muzzleFlareScale, 0.1f, 4.0f)
    RTB_PROPERTY_RANGE(fadeOutDuration, 0.0f, 2.0f)
    RTB_PROPERTY_RANGE(tipBurstCount, 0, 128)
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
    RTB_PROPERTY_COMPONENT(tipParticles, ParticleSystem)
    RTB_PROPERTY_COMPONENT(muzzleParticles, ParticleSystem)
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
    radialSegments = std::clamp(radialSegments, 6, 32);
    coreWidthScale = std::max(0.05f, coreWidthScale);
    innerWidthScale = std::max(0.1f, innerWidthScale);
    outerWidthScale = std::max(0.1f, outerWidthScale);
    taperAmount = std::clamp(taperAmount, 0.0f, 0.95f);
    tipCapScale = std::max(0.1f, tipCapScale);
    muzzleFlareScale = std::max(0.1f, muzzleFlareScale);
    fadeOutDuration = std::max(0.0f, fadeOutDuration);
    tipBurstCount = std::clamp(tipBurstCount, 0, 128);
    softEdge = std::max(0.05f, softEdge);
    uvTilesPerMeter = std::max(0.0f, uvTilesPerMeter);
    emissionStrength = std::max(0.0f, emissionStrength);
    glowIntensity = std::max(0.0f, glowIntensity);
}

void EnergyBeamComponent::OnDestroy()
{
    StopEndpointEffects(false);
    ReleaseRenderResources();
}

void EnergyBeamComponent::OnUpdate(float deltaTime)
{
    const float dt = std::max(0.0f, deltaTime);

    if (playing || fadingOut) {
        animTime += dt;
        SyncEndpointEffects();
    }

    if (playing) {
        elapsed += dt;
        if (duration > 0.0f && elapsed >= duration) {
            BeginFadeOut();
        }
        return;
    }

    if (!fadingOut) {
        return;
    }

    if (fadeOutDuration <= 0.0f) {
        fadeAlpha = 0.0f;
    } else {
        fadeAlpha = std::max(0.0f, fadeAlpha - dt / fadeOutDuration);
    }

    if (fadeAlpha <= 0.001f) {
        fadingOut = false;
        fadeAlpha = 0.0f;
        ClearBeam();
        StopEndpointEffects(false);
        SetUpdateTickEnabled(false);
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
    fadingOut = false;
    fadeAlpha = 1.0f;
    elapsed = 0.0f;
    SetUpdateTickEnabled(true);
    StartEndpointEffects();
}

void EnergyBeamComponent::Stop()
{
    playing = false;
    fadingOut = false;
    fadeAlpha = 0.0f;
    ClearBeam();
    StopEndpointEffects(false);
    SetUpdateTickEnabled(false);
}

void EnergyBeamComponent::BeginFadeOut()
{
    if (!HasRenderableBeam()) {
        Stop();
        return;
    }

    if (fadingOut) {
        return;
    }

    playing = false;
    fadingOut = true;
    fadeAlpha = 1.0f;
    SetUpdateTickEnabled(true);
    StopEndpointEffects(true);
}

void EnergyBeamComponent::ApplyDefaults()
{
    length = 8.0f;
    radius = 0.28f;
    duration = 0.0f;
    useVerticalCross = false;
    radialSegments = 12;
    coreWidthScale = 0.40f;
    innerWidthScale = 0.72f;
    outerWidthScale = 1.10f;
    taperAmount = 0.55f;
    tipCapScale = 1.15f;
    muzzleFlareScale = 1.75f;
    fadeOutDuration = 0.22f;
    tipBurstCount = 22;
    beamColor = RTBEngine::Math::Vector4(0.25f, 0.78f, 1.00f, 0.55f);
    coreColor = RTBEngine::Math::Vector4(0.95f, 0.98f, 1.00f, 0.95f);
    emissionStrength = 1.35f;
    noiseScale = 3.5f;
    noiseSpeed = 2.2f;
    distortionStrength = 0.12f;
    fresnelPower = 2.2f;
    glowIntensity = 0.85f;
    pulseSpeed = 4.0f;
    pulseAmount = 0.12f;
    softEdge = 0.90f;
    uvScrollSpeed = 2.5f;
    uvTilesPerMeter = 0.45f;
}

bool EnergyBeamComponent::HasRenderableBeam() const
{
    return visible && points.size() >= 2 && (playing || fadingOut);
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
    device.EnableVertexAttribFloat(3, 3, static_cast<int>(sizeof(BeamVertex)), offsetof(BeamVertex, normal));
    device.EnableVertexAttribFloat(4, 1, static_cast<int>(sizeof(BeamVertex)), offsetof(BeamVertex, shell));
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

void EnergyBeamComponent::OrientEffect(
    RTBEngine::Scene::ParticleSystem* particles,
    const RTBEngine::Math::Vector3& position,
    const RTBEngine::Math::Vector3& direction) const
{
    if (!particles || !particles->GetOwner()) {
        return;
    }

    RTBEngine::Math::Vector3 dir = direction;
    if (dir.LengthSquared() <= kSegmentEpsilon) {
        dir = RTBEngine::Math::Vector3::Forward();
    } else {
        dir.Normalize();
    }

    const float yaw = std::atan2(dir.x, dir.z);
    const float pitch = -std::asin(std::clamp(dir.y, -1.0f, 1.0f));
    RTBEngine::Scene::Transform& transform = particles->GetOwner()->GetTransform();
    transform.SetPosition(position);
    transform.SetRotation(RTBEngine::Math::Quaternion::FromEulerAngles(pitch, yaw, 0.0f));
}

void EnergyBeamComponent::SyncEndpointEffects()
{
    if (points.size() < 2) {
        return;
    }

    RTBEngine::Math::Vector3 axis = points.back() - points.front();
    if (axis.LengthSquared() <= kSegmentEpsilon) {
        return;
    }
    axis.Normalize();

    OrientEffect(muzzleParticles, points.front(), axis);
    OrientEffect(tipParticles, points.back(), axis);
}

void EnergyBeamComponent::StartEndpointEffects()
{
    SyncEndpointEffects();
    endpointFxActive = true;

    if (muzzleParticles) {
        if (RTBEngine::Scene::GameObject* go = muzzleParticles->GetOwner()) {
            go->SetActive(true);
        }
        muzzleParticles->SetEnabled(true);
        muzzleParticles->Restart();
    }

    if (tipParticles) {
        if (RTBEngine::Scene::GameObject* go = tipParticles->GetOwner()) {
            go->SetActive(true);
        }
        tipParticles->SetEnabled(true);
        tipParticles->Restart();
    }
}

void EnergyBeamComponent::StopEndpointEffects(bool burstTip)
{
    if (tipParticles) {
        if (burstTip && tipBurstCount > 0) {
            tipParticles->Emit(tipBurstCount);
        }
        tipParticles->Stop();
    }

    if (muzzleParticles) {
        muzzleParticles->Stop();
    }

    endpointFxActive = false;
}

void EnergyBeamComponent::AppendTube(
    std::vector<BeamVertex>& outVertices,
    float radiusScale,
    const RTBEngine::Math::Vector4& color,
    float shell) const
{
    if (points.size() < 2 || radiusScale <= 0.0f) {
        return;
    }

    const int segments = std::clamp(radialSegments, 6, 32);
    const float scrollOffset = animTime * uvScrollSpeed;
    const float base = std::max(radius, kMinRadius) * radiusScale;
    const float tipScale = 1.0f - taperAmount;

    std::vector<float> cumulativeDistance(points.size(), 0.0f);
    for (std::size_t i = 1; i < points.size(); ++i) {
        cumulativeDistance[i] = cumulativeDistance[i - 1] + (points[i] - points[i - 1]).Length();
    }
    const float totalLength = std::max(cumulativeDistance.back(), 0.001f);

    auto radiusAt = [&](float distance) {
        const float t = std::clamp(distance / totalLength, 0.0f, 1.0f);
        return base * (1.0f + (tipScale - 1.0f) * t);
    };

    for (std::size_t i = 0; i + 1 < points.size(); ++i) {
        const RTBEngine::Math::Vector3 start = points[i];
        const RTBEngine::Math::Vector3 end = points[i + 1];

        RTBEngine::Math::Vector3 axis;
        RTBEngine::Math::Vector3 right;
        RTBEngine::Math::Vector3 up;
        if (!BuildFrame(end - start, axis, right, up)) {
            continue;
        }

        const float startU = cumulativeDistance[i] * uvTilesPerMeter + scrollOffset;
        const float endU = cumulativeDistance[i + 1] * uvTilesPerMeter + scrollOffset;
        const float radiusStart = std::max(radiusAt(cumulativeDistance[i]), kMinRadius);
        const float radiusEnd = std::max(radiusAt(cumulativeDistance[i + 1]), kMinRadius);

        auto ringNormal = [&](int s) {
            const float t = static_cast<float>(s) / static_cast<float>(segments);
            const float angle = t * (kPi * 2.0f);
            RTBEngine::Math::Vector3 normal = right * std::cos(angle) + up * std::sin(angle);
            normal.Normalize();
            return normal;
        };

        auto pushVertex = [&](const RTBEngine::Math::Vector3& position,
                              float u,
                              float v,
                              const RTBEngine::Math::Vector3& normal) {
            BeamVertex vertex{};
            vertex.position[0] = position.x;
            vertex.position[1] = position.y;
            vertex.position[2] = position.z;
            vertex.color[0] = color.x;
            vertex.color[1] = color.y;
            vertex.color[2] = color.z;
            vertex.color[3] = color.w;
            vertex.uv[0] = u;
            vertex.uv[1] = v;
            vertex.normal[0] = normal.x;
            vertex.normal[1] = normal.y;
            vertex.normal[2] = normal.z;
            vertex.shell = shell;
            outVertices.push_back(vertex);
        };

        for (int s = 0; s < segments; ++s) {
            const RTBEngine::Math::Vector3 n0 = ringNormal(s);
            const RTBEngine::Math::Vector3 n1 = ringNormal(s + 1);
            const float v0 = static_cast<float>(s) / static_cast<float>(segments);
            const float v1 = static_cast<float>(s + 1) / static_cast<float>(segments);

            const RTBEngine::Math::Vector3 p00 = start + n0 * radiusStart;
            const RTBEngine::Math::Vector3 p01 = start + n1 * radiusStart;
            const RTBEngine::Math::Vector3 p11 = end + n1 * radiusEnd;
            const RTBEngine::Math::Vector3 p10 = end + n0 * radiusEnd;

            pushVertex(p00, startU, v0, n0);
            pushVertex(p01, startU, v1, n1);
            pushVertex(p11, endU, v1, n1);

            pushVertex(p00, startU, v0, n0);
            pushVertex(p11, endU, v1, n1);
            pushVertex(p10, endU, v0, n0);
        }
    }
}

void EnergyBeamComponent::AppendHemisphere(
    std::vector<BeamVertex>& outVertices,
    const RTBEngine::Math::Vector3& center,
    const RTBEngine::Math::Vector3& axis,
    float hemisphereRadius,
    const RTBEngine::Math::Vector4& color,
    float shell) const
{
    if (hemisphereRadius <= kMinRadius) {
        return;
    }

    RTBEngine::Math::Vector3 forward;
    RTBEngine::Math::Vector3 right;
    RTBEngine::Math::Vector3 up;
    if (!BuildFrame(axis, forward, right, up)) {
        return;
    }

    const int segments = std::clamp(radialSegments, 6, 32);
    const int rings = std::max(3, segments / 3);

    auto pushVertex = [&](const RTBEngine::Math::Vector3& position,
                          const RTBEngine::Math::Vector3& normal,
                          float u,
                          float v) {
        BeamVertex vertex{};
        vertex.position[0] = position.x;
        vertex.position[1] = position.y;
        vertex.position[2] = position.z;
        vertex.color[0] = color.x;
        vertex.color[1] = color.y;
        vertex.color[2] = color.z;
        vertex.color[3] = color.w;
        vertex.uv[0] = u;
        vertex.uv[1] = v;
        vertex.normal[0] = normal.x;
        vertex.normal[1] = normal.y;
        vertex.normal[2] = normal.z;
        vertex.shell = shell;
        outVertices.push_back(vertex);
    };

    auto sample = [&](int ring, int seg, RTBEngine::Math::Vector3& outPos, RTBEngine::Math::Vector3& outNormal) {
        const float ringT = static_cast<float>(ring) / static_cast<float>(rings);
        const float phi = ringT * (kPi * 0.5f); // 0 equator -> pi/2 pole
        const float segT = static_cast<float>(seg) / static_cast<float>(segments);
        const float theta = segT * (kPi * 2.0f);
        const float cosPhi = std::cos(phi);
        const float sinPhi = std::sin(phi);
        outNormal = (right * (std::cos(theta) * cosPhi) + up * (std::sin(theta) * cosPhi) + forward * sinPhi);
        outNormal.Normalize();
        outPos = center + outNormal * hemisphereRadius;
    };

    for (int ring = 0; ring < rings; ++ring) {
        for (int seg = 0; seg < segments; ++seg) {
            RTBEngine::Math::Vector3 p00, n00, p01, n01, p10, n10, p11, n11;
            sample(ring, seg, p00, n00);
            sample(ring, seg + 1, p01, n01);
            sample(ring + 1, seg, p10, n10);
            sample(ring + 1, seg + 1, p11, n11);

            const float u0 = static_cast<float>(seg) / static_cast<float>(segments);
            const float u1 = static_cast<float>(seg + 1) / static_cast<float>(segments);
            const float v0 = static_cast<float>(ring) / static_cast<float>(rings);
            const float v1 = static_cast<float>(ring + 1) / static_cast<float>(rings);

            pushVertex(p00, n00, u0, v0);
            pushVertex(p01, n01, u1, v0);
            pushVertex(p11, n11, u1, v1);

            pushVertex(p00, n00, u0, v0);
            pushVertex(p11, n11, u1, v1);
            pushVertex(p10, n10, u0, v1);
        }
    }
}

void EnergyBeamComponent::AppendCaps(std::vector<BeamVertex>& outVertices, float fade) const
{
    if (points.size() < 2) {
        return;
    }

    RTBEngine::Math::Vector3 axis = points.back() - points.front();
    if (axis.LengthSquared() <= kSegmentEpsilon) {
        return;
    }
    axis.Normalize();

    const float base = std::max(radius, kMinRadius);
    const float tipRadius = base * outerWidthScale * (1.0f - taperAmount) * tipCapScale;
    const float muzzleRadius = base * outerWidthScale * muzzleFlareScale;

    RTBEngine::Math::Vector4 tipColor = coreColor;
    tipColor.w *= 0.85f * fade;
    RTBEngine::Math::Vector4 muzzleColor = beamColor;
    muzzleColor.w *= 0.40f * fade;

    AppendHemisphere(outVertices, points.back(), axis, tipRadius, tipColor, 0.0f);
    AppendHemisphere(outVertices, points.front(), axis * -1.0f, muzzleRadius, muzzleColor, 2.0f);
}

void EnergyBeamComponent::Render(RTBEngine::Rendering::Camera* camera)
{
    if (!isEnabled || !HasRenderableBeam() || !camera) {
        return;
    }
    if (!GetOwner() || !GetOwner()->IsActiveInHierarchy()) {
        return;
    }
    if (!EnsureRenderResources()) {
        return;
    }

    const float fade = fadingOut ? fadeAlpha : 1.0f;
    std::vector<BeamVertex> vertices;
    const int segments = std::clamp(radialSegments, 6, 32);
    vertices.reserve((points.size() - 1) * static_cast<std::size_t>(segments) * 6 * 3 + 512);

    RTBEngine::Math::Vector4 outerColor = beamColor;
    outerColor.w *= 0.45f * fade;
    RTBEngine::Math::Vector4 innerColor = beamColor;
    innerColor.w *= 0.75f * fade;
    RTBEngine::Math::Vector4 core = coreColor;
    core.w *= fade;

    AppendTube(vertices, outerWidthScale, outerColor, 2.0f);
    AppendTube(vertices, innerWidthScale, innerColor, 1.0f);
    AppendTube(vertices, coreWidthScale, core, 0.0f);
    AppendCaps(vertices, fade);

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
    shader->SetFloat("uEmissionStrength", emissionStrength * (0.55f + 0.45f * fade));
    shader->SetFloat("uNoiseScale", noiseScale);
    shader->SetFloat("uNoiseSpeed", noiseSpeed);
    shader->SetFloat("uDistortionStrength", distortionStrength);
    shader->SetFloat("uFresnelPower", fresnelPower);
    shader->SetFloat("uGlowIntensity", glowIntensity * fade);
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
