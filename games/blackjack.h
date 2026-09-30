#pragma once
#include <random>
#include "game/game.h"
#include "card/card.h" 

class BlackJack : public CasinoGame {
    private:
        Card m_deck {};

        struct RoundState {
            Outcome result {Outcome::PlayerLose};
            PlayingCard firstDrawDealer {};
            PlayingCard secondDrawDealer {};
            PlayingCard firstDrawPlayer {};
            PlayingCard secondDrawPlayer {};
            int dealerTotal = 0;
            int dealerNumAces = 0;
            int playerTotal = 0;
            int playerNumAces = 0;
        };

    public:
        std::string getGameName() const override;

        void cleanupGame() override;

        // Ace, 2, 3, 4, 5, 6, 7, 8, 9, 10, J, Q, K.
        int getCardValue(PlayingCard card);

        void addCard(int& total, int& numAces, PlayingCard card);

         void setup(Player& p) override;

        void drawCards(RoundState& state);

        bool checkNatural(Player& p, RoundState& state);

        void printDrawnCards(Player& p, RoundState& state);

        char askPlayerChoice();

        void drawAdditionalCardForPlayer(RoundState& state);

        bool checkIfPlayerBust(Player& p, RoundState& state);

        void dealerTurn(RoundState& state);

        void checkOutcome(RoundState& state);

        void playerTurn(RoundState& state);

        bool isBlackjack(Player& p, RoundState& state);

        void play(Player& p) override;
        
    };