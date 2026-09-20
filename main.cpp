#include <iostream>
#include <string>
#include <limits>
#include <cassert>
#include <cctype>
#include <random>
#include <vector>
#include <unordered_set>
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

class Card {
    private:
        std::vector<int> m_deck {0,1,2,3,4,5,6,7,8,9,10,11,12}; // jack, queen, king is all 10, Ace = 1 or 11 in blackjack.
        std::vector<int> m_suits {1,2,3,4};//clubs, spades, hearts, diamonds. // TODO: deal with later
        std::random_device rd;
        std::mt19937 gen{ rd() };
        const int exitNumber = 999;

    public:
        int getRandomCardNumber() {
            // 1. Define the distribution range based on the vector's indices
            // uniform_int_distribution is inclusive: [0, numbers.size() - 1]
            // assert(m_deck.size() > 0);
            if (m_deck.size() <= 0) {
                return exitNumber;
            }
            std::uniform_int_distribution<size_t> distr(0, m_deck.size() - 1);

            // 2. Generate a random index
            int random_index = distr(gen);
            
            //return shuold be the index, which is the card number
            return random_index;
        }

        int drawCard() {
            int index = getRandomCardNumber();
            if (index == exitNumber) {
                std::cout << "The deck is not valid! \n";
                return -1;
            }

            int temp = m_deck[index]; //save card before deleting it
            m_deck.erase(m_deck.begin() + index);
            return temp;
        }

        // i know its long,  refractor later
        int cardValue(int rawCard) {
            switch (rawCard) {
                case 0:
                    return 1;
                case 1:
                    // choice of 1 or 11
                    return 1; // hardcode 1 for now
                case 2:
                    return 2;
                case 3:
                    return 3;
                case 4:
                    return 4;
                case 5:
                    return 5;
                case 6:
                    return 6;
                case 7:
                    return 7;
                case 8:
                    return 8;
                case 9:
                    return 9;
                case 10:
                    return 10;
                case 11:
                    return 10;
                case 12:
                    return 10;
            }
            return -1;
        }

        void resetDeck() {
            m_deck = {0,1,2,3,4,5,6,7,8,9,10,11,12};
        }
};

class BlackJack : public CasinoGame {
    private:
        bool m_win {false};
        // random         
        std::mt19937 m_rng{std::random_device{}()}; // engine
        std::bernoulli_distribution m_dist{0.5}; // 50/50, need to change it to pick from a random range from 0-12.
        Card m_deck {};

    public:
        std::string getGameName() const {
            return "BlackJack";
        }

        void play(Player& p) {
            m_win = false;
            // bool coin = m_dist(m_rng); // true or false

            std::cout << "Welcome to BlackJack!\n";
            askBet(p);

            //rules.
            // dealer

            int dealerTotal = 0;
            int firstDealer = m_deck.drawCard();
            int secondDealer = m_deck.drawCard();
            dealerTotal += m_deck.cardValue(firstDealer);
            // The dealer receives one card face‑up and one face‑down (the “hole” card).
            dealerTotal += m_deck.cardValue(secondDealer);
            
            //player
            int playerTotal = 0;
            // 1. draws two cards 
            int firstPlayer = m_deck.drawCard();
            int secondPlayer = m_deck.drawCard();
            playerTotal += m_deck.cardValue(firstPlayer);
            playerTotal += m_deck.cardValue(secondPlayer);

            while (playerTotal <= 21) {
                // hit or stand
                
                std::cout << "Would you like to Hit or Stand? \n";
                char playerChoice;
                if (!(std::cin >> playerChoice)) {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    continue;
                }

                playerChoice = std::toupper(playerChoice);
                if (playerChoice == 'H') {
                    // draw card
                    
                } else if (playerChoice == 'S') {
                    // dealer picks up until they either go above your playerTotal or they bust

                } else {
                    // not H or S. break for now.
                    break;
                }
            }
            // hit = draw another card
            // stand = end turn
            
            // until 21?
            // m_deck.drawCard();
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