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
    bool IsPlaying() const { return playing; }

    void ApplyDefaults();

    float length = 8.0f;
    float radius = 0.85f;
    float duration = 0.0f;
    bool playing = false;
    bool visible = true;
    bool simulateInEditMode = true;
    bool useVerticalCross = true;

    float coreWidthScale = 0.55f;
    float innerWidthScale = 1.00f;
    float outerWidthScale = 1.65f;

    RTBEngine::Math::Vector4 beamColor = RTBEngine::Math::Vector4(0.35f, 0.85f, 1.00f, 0.85f);
    RTBEngine::Math::Vector4 coreColor = RTBEngine::Math::Vector4(1.00f, 0.98f, 0.94f, 1.00f);
    float emissionStrength = 2.5f;
    float noiseScale = 3.5f;
    float noiseSpeed = 2.2f;
    float distortionStrength = 0.15f;
    float fresnelPower = 2.5f;
    float glowIntensity = 1.4f;
    float pulseSpeed = 4.0f;
    float pulseAmount = 0.18f;
    float softEdge = 0.85f;
    float uvScrollSpeed = 2.5f;
    float uvTilesPerMeter = 0.35f;

    std::string shaderRef = "energy_beam";
    std::string vertexPath = "Assets/Shaders/EnergyBeam.vert";
    std::string fragmentPath = "Assets/Shaders/EnergyBeam.frag";

    RTB_COMPONENT(EnergyBeamComponent)

private:
    struct BeamVertex {
        float position[3];
        float color[4];
        float uv[2];
        float side;
    };

    std::vector<RTBEngine::Math::Vector3> points;
    float elapsed = 0.0f;
    float animTime = 0.0f;

    RTBEngine::Rendering::RHI::GpuId vao = RTBEngine::Rendering::RHI::kInvalidGpuId;
    RTBEngine::Rendering::RHI::GpuId vbo = RTBEngine::Rendering::RHI::kInvalidGpuId;
    RTBEngine::Rendering::Shader* shader = nullptr;

    bool EnsureRenderResources();
    void ReleaseRenderResources();
    void AppendRibbon(
        std::vector<BeamVertex>& outVertices,
        RTBEngine::Rendering::Camera* camera,
        float width,
        const RTBEngine::Math::Vector4& color,
        bool vertical) const;
    void Render(RTBEngine::Rendering::Camera* camera);
};
