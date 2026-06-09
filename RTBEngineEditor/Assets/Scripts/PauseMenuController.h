#pragma once

#include <RTBEngine/ECS/Component.h>
#include <RTBEngine/ECS/GameObject.h>
#include <RTBEngine/Reflection/PropertyMacros.h>
#include <RTBEngine/UI/UIElement.h>
#include <vector>

namespace RTBEngine {
    namespace UI {
        class UIButton;
        class UIText;
    }
}

class PauseMenuController : public RTBEngine::ECS::Component
{
public:
    PauseMenuController() = default;
    ~PauseMenuController() override = default;

    void OnAwake() override;
    void OnStart() override;
    void OnUpdate(float deltaTime) override;
    void OnValidate() override;
    void OnDestroy() override;

    RTBEngine::ECS::GameObject* menuRoot = nullptr;
    RTBEngine::UI::UIButton* resumeButton = nullptr;
    RTBEngine::UI::UIButton* exitButton = nullptr;
    RTBEngine::UI::UIText* notificationText = nullptr;
    bool pauseSimulation = false;
    bool useRelativeMouseWhenClosed = true;
    std::string mainMenuScenePath = "Assets/Scenes/MainMenu.lua";

    RTB_COMPONENT(PauseMenuController)

public:
    void TogglePause();
    void PauseGame();
    void ResumeGame();
    void ExitToMainMenu();
    bool IsMenuOpen() const { return menuVisible; }
    static bool IsAnyMenuOpen();

private:
    struct UIElementState {
        RTBEngine::UI::UIElement* element = nullptr;
        bool visible = true;
        bool raycastTarget = true;
    };

    std::vector<UIElementState> capturedElementStates;
    bool capturedInitialState = false;
    bool menuVisible = false;
    bool resumeButtonBound = false;
    bool exitButtonBound = false;

    void BindButtons();
    void RefreshMatchNotification();
    void CaptureMenuElementStates();
    void CaptureMenuElementStatesRecursive(RTBEngine::ECS::GameObject* root);
    void SetMenuVisible(bool visible);
    void ApplyMouseModeForMenuState();
};
