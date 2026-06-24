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

    std::string idleAnimationFbx =

        "Assets/3D/KayKit_Character_Animations/Animations/fbx/Rig_Medium/Rig_Medium_General.fbx|Idle_A";



    RTB_COMPONENT(MainMenuCharacterPreview)

public:
    void OnStart() override;
    void OnDestroy() override;
    void ShowCharacterById(const std::string& characterId);

private:

    RTBEngine::ECS::GameObject* spawnedPreview = nullptr;

    std::string loadedModelRef;



    void ClearPreview();

    void ShowDefinition(const CharacterDefinition& definition);

    void ApplyPreviewTransform();

    void PlayIdleAnimation();

};


