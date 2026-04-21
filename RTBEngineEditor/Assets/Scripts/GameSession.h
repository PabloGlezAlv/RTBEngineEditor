#pragma once

enum class GameResult {
    None,
    Win,
    Lose
};

class GameSession {
public:
    static GameSession& GetInstance();

    void Reset();
    void SetResult(GameResult result);
    GameResult GetResult() const;

private:
    GameSession() = default;

    GameResult result = GameResult::None;
};
