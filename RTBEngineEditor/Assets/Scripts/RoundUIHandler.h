#pragma once

#include <RTBEngine/Scene/Component.h>
#include <RTBEngine/Reflection/PropertyMacros.h>
#include <RTBEngine/UI/Elements/UIText.h>
#include <string>

class RoundUIHandler : public RTBEngine::ECS::Component
{
public:
    RoundUIHandler() = default;
    ~RoundUIHandler() override = default;

    void OnStart() override;
    void OnValidate() override;

    RTBEngine::UI::UIText* roundText = nullptr;
    RTBEngine::UI::UIText* countdownText = nullptr;
    std::string roundPrefix = "Round ";
    std::string countdownPrefix = "Start in ";
    std::string endGameCountdownPrefix = "Game over in ";
    std::string respawnCountdownPrefix = "Revive in ";

    RTB_COMPONENT(RoundUIHandler)

public:
    void ShowRound(int roundNumber);
    void ShowCountdown(int secondsRemaining);
    void HideCountdown();
    void ShowEndGameCountdown(int secondsRemaining);
    void HideEndGameCountdown();
    void ShowRespawnCountdown(int secondsRemaining);
    void HideRespawnCountdown();

private:
    int currentRound = 0;
    int currentCountdownSeconds = 0;
    bool countdownVisible = false;
    int currentEndGameSeconds = 0;
    bool endGameCountdownVisible = false;
    int currentRespawnSeconds = 0;
    bool respawnCountdownVisible = false;

    void ApplyState() const;
};
