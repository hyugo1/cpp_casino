#include <iostream>
#include <string>
#include <limits>
#include <cctype>
#include <random>

class Player {
    private:
        std::string m_name {};
        int m_bal {100};
        int m_total_games_played {0};
        int m_wins {0};
        int m_losses {0};

    public:
        Player(std::string name="Guest", int bal=100, int games_played=0, int wins=0) : m_name{name}, m_bal {bal}, m_total_games_played {games_played}, m_wins {wins} {};

        const std::string_view getName() const { return m_name; }
        int getBalance() const { return m_bal; }
        double getWinRatio() const { 
            if (m_total_games_played == 0) return 0.0;
            return (static_cast<double>(m_wins) / m_total_games_played) * 100;
        }

        void addToBalance(int amount) { m_bal += amount; }
        void subFromBalance(int amount) { m_bal -= amount; }
        void recordResult(bool won) {
            if (won) { ++m_wins; }
            else { ++m_losses; }
            ++m_total_games_played;
        }
};

class CasinoGame {
    public:
        virtual void play(Player& p) = 0;
        virtual std::string getGameName() const = 0;
        virtual ~CasinoGame() = default;
        int m_bet_amount {std::numeric_limits<int>::max()};
        int m_multiplier {3};
    protected:
        void askBet(Player& p) {
            do {
                std::cout << "How much are you betting? "; 
                if (!(std::cin >> m_bet_amount)) {
                    std::cout << "Invalid input. Please enter a number.\n";
                    std::cin.clear(); 
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 
                    continue;
                }
                if (m_bet_amount > p.getBalance()) {
                    std::cout << "You can't afford that. Try again\n";
                }
            } while (m_bet_amount > p.getBalance() || m_bet_amount <= 0);
            std::cout << "Betted: " << m_bet_amount << '\n';
            p.subFromBalance(m_bet_amount);

        }

        void awardRewards(Player& p) {
            p.addToBalance(m_bet_amount * m_multiplier);
        }
};

class CoinFlip : public CasinoGame {
    private:
        bool m_win {false};
        std::mt19937 m_rng{std::random_device{}()}; // engine
        std::bernoulli_distribution m_dist{0.5};// 50/50
        
    public:
        std::string getGameName() const {
            return "CoinFlip";
        }

        void play(Player& p) {
            m_win = false;
            bool coin = m_dist(m_rng); // true or false
            char bot_decision = coin ? 'H' : 'T';

            std::cout << "Welcome to Heads or Tails!\n";
            askBet(p);
            
            char guess;
            while (true) {
                std::cout << "Heads or Tails? Type H or T.\n";
                if (!(std::cin >> guess)) {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    continue;
                }

                guess = std::toupper(guess);
                if (guess == 'T' || guess == 'H') {
                    // try again???
                    break;
                }
                std::cout << "Invalid input. Please enter H or T.\n";
            }

            if (guess == bot_decision) {
                m_win = true;
                std::cout << "You win!\n";
            } else {
                std::cout << "You lose! It was " << bot_decision << "\n";
            }
            p.recordResult(m_win);

            if (m_win) {
                awardRewards(p);
            }
        }
};

int main() { 
    Player player {};
    std::cout << "Hello, " << player.getName() << "!\n";
    std::cout << "Your current balance is: " << player.getBalance() << '\n';
    std::cout << "Your current win rate is: " << player.getWinRatio() << "%\n";

    CoinFlip c {};
    while (true) {
        c.play(player);
        std::cout << "Your current balance is: " << player.getBalance() << '\n';
    }

    return 0;
}