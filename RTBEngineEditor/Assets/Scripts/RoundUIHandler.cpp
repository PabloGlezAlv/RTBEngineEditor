#include "RoundUIHandler.h"

#include <algorithm>

using ThisClass = RoundUIHandler;

RTB_REGISTER_COMPONENT(RoundUIHandler)
    RTB_PROPERTY_COMPONENT(roundText, UIText)
    RTB_PROPERTY_COMPONENT(countdownText, UIText)
    RTB_PROPERTY(roundPrefix)
    RTB_PROPERTY(countdownPrefix)
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

    if (!countdownVisible) {
        countdownText->SetText("");
        return;
    }

    countdownText->SetText(countdownPrefix + std::to_string(currentCountdownSeconds));
}
