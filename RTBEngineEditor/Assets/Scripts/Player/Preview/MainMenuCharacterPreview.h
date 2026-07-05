#pragma once

#include <RTBEngine/Math/Vectors/Vector3.h>
#include <RTBEngine/Scene/Component.h>
#include <RTBEngine/Scene/GameObject.h>
#include <RTBEngine/Reflection/PropertyMacros.h>

#include <string>

class CharacterDefinition;

class MainMenuCharacterPreview : public RTBEngine::ECS::Component {
public:
    MainMenuCharacterPreview() = default;
    ~MainMenuCharacterPreview() override = default;

    RTBEngine::Math::Vector3 previewOffset = RTBEngine::Math::Vector3(0.0f, 0.0f, 0.0f);
    float previewYawDegrees = 0.0f;
    float previewScale = 0.85f;

    RTB_COMPONENT(MainMenuCharacterPreview)

public:
    void OnDestroy() override;
    void ShowCharacterById(const std::string& characterId);

private:
    RTBEngine::ECS::GameObject* previewInstance = nullptr;
    std::string currentCharacterId;

    void ClearPreviewInstance();
    void ApplyPreviewTransform();
    void PlayIdleAnimation();
};
