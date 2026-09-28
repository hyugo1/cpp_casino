// coinflip.cpp
#include <iostream>
#include <string>
#include <vector>

#include "coinflip.h"
#include "utils/utils.h"

std::string CoinFlip::getGameName() const {
    return "CoinFlip";
}

// void cleanupGame() override {
// }

void CoinFlip::play(Player& p) {
    bool coin = m_dist(m_rng); // true or false
    char bot_decision = coin ? 'H' : 'T';

    std::cout << "🎴 Welcome to Heads or Tails!\n";
    askBet(p);
    
    char guess;
    while (true) {
        std::cout << "Heads or Tails? Type H or T: ";
        if (!Utils::tryRead(guess)) {continue;}

        guess = std::toupper(guess);
        if (guess == 'T' || guess == 'H') {
            // try again???
            break;
        }
        std::cout << "❌ Invalid input. Please enter H or T.\n";
    }

    if (guess == bot_decision) {
        resolveOutcome(Outcome::PlayerWin, p);
    } else {
        resolveOutcome(Outcome::PlayerLose, p);
    }
}
