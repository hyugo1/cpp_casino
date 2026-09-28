// game.h
#pragma once
#include <iostream>
#include <string>
#include <vector>

class Player;

class CasinoGame {
    public:
        // virtual void setup(Player& p) = 0;
        virtual void setup(Player& p) {};
        virtual void play(Player& p) = 0;
        virtual void cleanupGame() {};
        virtual std::string getGameName() const = 0;
        virtual ~CasinoGame() = default;
        
        enum class Outcome { PlayerBust, DealerBust, PlayerNatural, DealerNatural, Draw, PlayerWin, PlayerLose };
        
    private:
        bool m_win {false};
        bool m_draw {false};
        double m_bet_amount {0};
        double m_multiplier {2.0};

    protected:
        void askBet(Player& p);

        void awardRewards(Player& p);

        void setMultiplier(double mul);
        
        void resetMultiplier();

        void refundBet(Player& p);

        void resolveOutcome(Outcome result, Player& p);
};

