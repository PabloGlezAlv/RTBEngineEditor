#include "GameSession.h"

GameSession& GameSession::GetInstance()
{
    static GameSession instance;
    return instance;
}

void GameSession::Reset()
{
    result = GameResult::None;
}

void GameSession::SetResult(GameResult newResult)
{
    result = newResult;
}

GameResult GameSession::GetResult() const
{
    return result;
}
