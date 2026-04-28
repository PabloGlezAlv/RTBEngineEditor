#pragma once

#include <RTBEngine/ECS/Component.h>
#include <RTBEngine/ECS/GameObject.h>
#include <RTBEngine/Reflection/PropertyMacros.h>
#include <RTBEngine/UI/UIElement.h>
#include <vector>

class PauseMenuController : public RTBEngine::ECS::Component
{
public:
    PauseMenuController() = default;
    ~PauseMenuController() override = default;

    void OnAwake() override;
    void OnStart() override;
    void OnUpdate(float deltaTime) override;

    RTBEngine::ECS::GameObject* menuRoot = nullptr;

    RTB_COMPONENT(PauseMenuController)

public:
    void TogglePause();
    void PauseGame();
    void ResumeGame();
    bool IsMenuOpen() const { return menuVisible; }

private:
    struct UIElementState {
        RTBEngine::UI::UIElement* element = nullptr;
        bool visible = true;
        bool raycastTarget = true;
    };

    std::vector<UIElementState> capturedElementStates;
    bool capturedInitialState = false;
    bool menuVisible = false;

    void CaptureMenuElementStates();
    void CaptureMenuElementStatesRecursive(RTBEngine::ECS::GameObject* root);
    void SetMenuVisible(bool visible);
};
