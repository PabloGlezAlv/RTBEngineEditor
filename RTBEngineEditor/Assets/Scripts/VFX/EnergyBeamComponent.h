#pragma once

#include <RTBEngine/Math/Vectors/Vector3.h>
#include <RTBEngine/Math/Vectors/Vector4.h>
#include <RTBEngine/Reflection/PropertyMacros.h>
#include <RTBEngine/Rendering/RHI/RenderTypes.h>
#include <RTBEngine/Scene/Component.h>

#include <cstddef>
#include <string>
#include <vector>

namespace RTBEngine {
    namespace Rendering {
        class Camera;
        class Shader;
    }
    namespace Scene {
        class ParticleSystem;
    }
}

// Gameplay VFX: lives in GameScripts, not the engine core.
class EnergyBeamComponent : public RTBEngine::Scene::Component {
public:
    EnergyBeamComponent();
    ~EnergyBeamComponent() override;

    EnergyBeamComponent(const EnergyBeamComponent&) = delete;
    EnergyBeamComponent& operator=(const EnergyBeamComponent&) = delete;

    void OnValidate() override;
    void OnDestroy() override;
    void OnUpdate(float deltaTime) override;

    bool WantsTransparentRender() const override { return true; }
    void OnTransparentRender(RTBEngine::Rendering::Camera* camera) override;

    bool WantsEditModeSimulate() const override { return simulateInEditMode; }
    void OnEditModeSimulate(float deltaTime) override;

    void SetEndpoints(const RTBEngine::Math::Vector3& start, const RTBEngine::Math::Vector3& end);
    void SetBeam(
        const RTBEngine::Math::Vector3& origin,
        const RTBEngine::Math::Vector3& direction,
        float length);
    void ClearBeam();

    void Play();
    void Stop();
    void BeginFadeOut();
    bool IsPlaying() const { return playing; }
    bool IsFadingOut() const { return fadingOut; }

    void ApplyDefaults();

    float length = 8.0f;
    float radius = 0.28f;
    float duration = 0.0f;
    bool playing = false;
    bool visible = true;
    bool simulateInEditMode = true;
    bool useVerticalCross = false;
    int radialSegments = 12;

    float coreWidthScale = 0.40f;
    float innerWidthScale = 0.72f;
    float outerWidthScale = 1.10f;
    float taperAmount = 0.55f;
    float tipCapScale = 1.15f;
    float muzzleFlareScale = 1.75f;
    float fadeOutDuration = 0.22f;
    int tipBurstCount = 22;

    RTBEngine::Math::Vector4 beamColor = RTBEngine::Math::Vector4(0.25f, 0.78f, 1.00f, 0.55f);
    RTBEngine::Math::Vector4 coreColor = RTBEngine::Math::Vector4(0.95f, 0.98f, 1.00f, 0.95f);
    float emissionStrength = 1.35f;
    float noiseScale = 3.5f;
    float noiseSpeed = 2.2f;
    float distortionStrength = 0.12f;
    float fresnelPower = 2.2f;
    float glowIntensity = 0.85f;
    float pulseSpeed = 4.0f;
    float pulseAmount = 0.12f;
    float softEdge = 0.90f;
    float uvScrollSpeed = 2.5f;
    float uvTilesPerMeter = 0.45f;

    RTBEngine::Scene::ParticleSystem* tipParticles = nullptr;
    RTBEngine::Scene::ParticleSystem* muzzleParticles = nullptr;

    std::string shaderRef = "energy_beam";
    std::string vertexPath = "Assets/Shaders/EnergyBeam.vert";
    std::string fragmentPath = "Assets/Shaders/EnergyBeam.frag";

    RTB_COMPONENT(EnergyBeamComponent)

private:
    struct BeamVertex {
        float position[3];
        float color[4];
        float uv[2];
        float normal[3];
        float shell;
    };

    std::vector<RTBEngine::Math::Vector3> points;
    float elapsed = 0.0f;
    float animTime = 0.0f;
    bool fadingOut = false;
    float fadeAlpha = 1.0f;
    bool endpointFxActive = false;

    RTBEngine::Rendering::RHI::GpuId vao = RTBEngine::Rendering::RHI::kInvalidGpuId;
    RTBEngine::Rendering::RHI::GpuId vbo = RTBEngine::Rendering::RHI::kInvalidGpuId;
    RTBEngine::Rendering::Shader* shader = nullptr;

    bool EnsureRenderResources();
    void ReleaseRenderResources();
    void AppendTube(
        std::vector<BeamVertex>& outVertices,
        float radiusScale,
        const RTBEngine::Math::Vector4& color,
        float shell) const;
    void AppendHemisphere(
        std::vector<BeamVertex>& outVertices,
        const RTBEngine::Math::Vector3& center,
        const RTBEngine::Math::Vector3& axis,
        float hemisphereRadius,
        const RTBEngine::Math::Vector4& color,
        float shell) const;
    void AppendCaps(std::vector<BeamVertex>& outVertices, float fade) const;
    void SyncEndpointEffects();
    void StartEndpointEffects();
    void StopEndpointEffects(bool burstTip);
    void OrientEffect(
        RTBEngine::Scene::ParticleSystem* particles,
        const RTBEngine::Math::Vector3& position,
        const RTBEngine::Math::Vector3& direction) const;
    void Render(RTBEngine::Rendering::Camera* camera);
    bool HasRenderableBeam() const;
};
