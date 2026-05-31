#include "RoundUIHandler.h"

#include <algorithm>

using ThisClass = RoundUIHandler;

RTB_REGISTER_COMPONENT(RoundUIHandler)
    RTB_PROPERTY_COMPONENT(roundText, UIText)
    RTB_PROPERTY_COMPONENT(countdownText, UIText)
    RTB_PROPERTY(roundPrefix)
    RTB_PROPERTY(countdownPrefix)
    RTB_PROPERTY(endGameCountdownPrefix)
    RTB_PROPERTY(respawnCountdownPrefix)
RTB_END_REGISTER(RoundUIHandler)

void RoundUIHandler::OnStart()
{
    ApplyState();
}

void RoundUIHandler::OnValidate()
{
    ApplyState();
}

void RoundUIHandler::ShowRound(int roundNumber)
{
    currentRound = std::max(0, roundNumber);
    ApplyState();
}

void RoundUIHandler::ShowCountdown(int secondsRemaining)
{
    currentCountdownSeconds = std::max(0, secondsRemaining);
    countdownVisible = true;
    ApplyState();
}

void RoundUIHandler::HideCountdown()
{
    countdownVisible = false;
    ApplyState();
}

void RoundUIHandler::ShowEndGameCountdown(int secondsRemaining)
{
    currentEndGameSeconds = std::max(0, secondsRemaining);
    endGameCountdownVisible = true;
    ApplyState();
}

void RoundUIHandler::HideEndGameCountdown()
{
    endGameCountdownVisible = false;
    ApplyState();
}

void RoundUIHandler::ShowRespawnCountdown(int secondsRemaining)
{
    currentRespawnSeconds = std::max(0, secondsRemaining);
    respawnCountdownVisible = true;
    ApplyState();
}

void RoundUIHandler::HideRespawnCountdown()
{
    respawnCountdownVisible = false;
    ApplyState();
}

void RoundUIHandler::ApplyState() const
{
    if (roundText) {
        if (currentRound > 0) {
            roundText->SetText(roundPrefix + std::to_string(currentRound));
        } else {
            roundText->SetText("");
        }
    }

    if (!countdownText) {
        return;
    }

    if (endGameCountdownVisible) {
        countdownText->SetText(endGameCountdownPrefix + std::to_string(currentEndGameSeconds));
        return;
    }

    if (respawnCountdownVisible) {
        countdownText->SetText(respawnCountdownPrefix + std::to_string(currentRespawnSeconds));
        return;
    }

    if (!countdownVisible) {
        countdownText->SetText("");
        return;
    }

    countdownText->SetText(countdownPrefix + std::to_string(currentCountdownSeconds));
}
