#pragma once
#include <random>
#include "game/game.h"

class GuessTheCard : public CasinoGame {
    private:
        Card m_deck {};
        PlayingCard card {};
        int m_numOflives {10};
        const std::array<std::string, 4> suits { "spades", "hearts", "diamonds", "clubs" };
        struct RoundState {
            Outcome result {Outcome::PlayerLose};
            int tries = 0;
            bool guessedCorrectly = false;
            bool hasGuessedSuit = false;
            bool hasGuessedRank = false;
            int confirmedRank = -1;
            std::string confirmedSuit;
        } ;

    public:
        std::string getGameName() const override;

        void cleanupGame() override;

        void setNumOfLives(int lives);

        void checkRankGuess(int rankGuess, int cardRank);

        int moveByOne(int rankGuess);

        int getRankGuess();

        std::string getSuitGuess();

        void runEasyMode(RoundState& state);

        void runStandardMode(RoundState& state);

        void setup(Player& p) override;

        void showAnswer(RoundState& state);

        void play(Player& p) override;
};
