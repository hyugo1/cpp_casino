#include <iostream>
#include <string>
#include <limits>

class Player {
    private:
        std::string m_name {};
        int m_bal {100};
        int m_total_games_played {1};
        int m_wins {1};

    public:
        Player(std::string name="Guest", int bal=100, int games_played=1, int wins=1) : m_name{name}, m_bal {bal}, m_total_games_played {games_played}, m_wins {wins} {};

        const std::string_view getName() const { return m_name; }
        int getBalance() const { return m_bal; }
        double getWinRatio() { return (static_cast<double>(m_wins) / m_total_games_played) * 100; }

        void addToBalance(int amount) { m_bal += amount; }
        void subFromBalance(int amount) { m_bal -= amount; }
        void recordResult(bool won) {
            if (won) { ++m_wins; }
        }
};

// take money away (if they lose) or give money (if they win)
// record a win/loss for the ratio stat
class CasinoGame {
    public:
        virtual void play(Player& p) = 0;
        virtual std::string getGameName() const = 0;
        virtual ~CasinoGame() = default;
        int m_bet_amount {std::numeric_limits<int>::max()};
        int m_multiplier {2};
    protected:
        void askBet(Player& p) {
            do {
                std::cout << "How much are you betting? "; 
                std::cin >> m_bet_amount;
                if (m_bet_amount > p.getBalance()) {
                    std::cout << "You can't afford that. Try again";
                }
            } while (m_bet_amount > p.getBalance());
            //  && m_bet_amount != int
            std::cout << "Betted: " << m_bet_amount << '\n';
            p.subFromBalance(m_bet_amount);

        }

        void awardRewards(bool winCondition, Player& p) {
            if (winCondition) {
                p.addToBalance(m_bet_amount*m_multiplier);
            }
        }
};

class CoinFlip : public CasinoGame {
    private:
        bool m_win {false};
        char bot_decision {'T'}; //FIXME: randomise later
    public:
        std::string getGameName() const {
            return "CoinFlip";
        }

        void play(Player& p) {
            std::cout << "Welcome to Heads or Tails!\n";
            askBet(p);
            
            std::cout << "Heads or Tails? Type H or T.\n";
            char guess;
            std::cin >> guess;
            
            if (guess == bot_decision) {
                m_win = true;
                p.recordResult(m_win);
            } else {
                std::cout << "You lose!\n";
            }

            if (m_win) {
                awardRewards(m_win, p);
            }
        }
};

int main() { 
    Player player {};
    std::cout << "Hello, " << player.getName() << "!\n";
    std::cout << "Your current balance is: " << player.getBalance() << '\n';
    std::cout << "Your current win rate is: " << player.getWinRatio() << "%\n";

    while (true) {
        CoinFlip c {};
        c.play(player);
        std::cout << "Your current balance is: " << player.getBalance() << '\n';
    }

    return 0;
}