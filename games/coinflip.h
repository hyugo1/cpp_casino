#pragma once
#include <random>
#include "game/game.h"

class CoinFlip : public CasinoGame {
private:
    std::mt19937 m_rng{std::random_device{}()}; // engine
    std::bernoulli_distribution m_dist{0.5};// 50/50

public:
    std::string getGameName() const override;
    // void cleanupGame() override {
    // }
    void play(Player& p) override;
};
