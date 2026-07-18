#pragma once



#include <RTBEngine/Core/Event.h>

#include <RTBEngine/Scene/Component.h>

#include <RTBEngine/Reflection/PropertyMacros.h>

#include <RTBEngine/UI/Elements/UIText.h>



class HealthBarUI;



class PlayerNameplateUI : public RTBEngine::Scene::Component

{

public:

    PlayerNameplateUI() = default;

    ~PlayerNameplateUI() override = default;



    RTBEngine::UI::UIText* displayNameText = nullptr;

    HealthBarUI* healthBarUI = nullptr;



    RTB_COMPONENT(PlayerNameplateUI)



public:

    void OnStart() override;

    void OnLateUpdate(float deltaTime) override;

    void OnDestroy() override;

    void OnValidate() override;



    void ForceRefreshDisplayName();

    void ApplyFixedWorldOrientation() const;



private:

    RTBEngine::Core::EventSubscription profileChangedSubscription;



    void RefreshDisplayName() const;

    void BindHealthBar();

    int ResolveOwnerPlayerSlot() const;

};

