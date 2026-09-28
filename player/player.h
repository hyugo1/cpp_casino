// player.h

#include <iostream>
#include <string>
#include <vector>

#include "utils/utils.h"

class Player {
    private:
        std::string m_name {};
        double m_bal {100};
        int m_total_games_played {0};
        int m_wins {0};
        int m_losses {0};
        std::vector<std::string> difficultyLevel {"easy", "medium", "hard"};
        int m_difficulty {4};

    public:
        Player(std::string name="Guest", double bal=100.0, int games_played=0, int wins=0);

        const std::string_view getName() const;
        void setName(const std::string& name);
        double getBalance() const;
        double getWinRatio() const;

        void addToBalance(double amount);
        void subFromBalance(double amount);
        void recordResult(bool won);

        void askDifficulty();
        
        int getDifficulty() const;
};