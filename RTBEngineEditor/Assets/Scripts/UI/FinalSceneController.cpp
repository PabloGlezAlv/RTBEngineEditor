#include "FinalSceneController.h"

#include "GameSession.h"

using ThisClass = FinalSceneController;

RTB_REGISTER_COMPONENT(FinalSceneController)
    RTB_PROPERTY_COMPONENT(resultText, UIText)
    RTB_PROPERTY(winMessage)
    RTB_PROPERTY(loseMessage)
    RTB_PROPERTY(fallbackMessage)
RTB_END_REGISTER(FinalSceneController)

void FinalSceneController::OnStart()
{
    ApplyResult();
}

void FinalSceneController::OnValidate()
{
    ApplyResult();
}

void FinalSceneController::ApplyResult() const
{
    if (!resultText) {
        return;
    }

    switch (GameSession::GetInstance().GetResult()) {
    case GameResult::Win:
        resultText->SetText(winMessage);
        break;
    case GameResult::Lose:
        resultText->SetText(loseMessage);
        break;
    case GameResult::None:
    default:
        resultText->SetText(fallbackMessage);
        break;
    }
}
