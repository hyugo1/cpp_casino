#include <iostream>
#include <string>
#include <limits>
#include <cassert>
#include <cctype>
#include <random>
#include <vector>
#include <unordered_set>
#include <memory>

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
        virtual void cleanupGame(Player& p) = 0;
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

        void cleanupGame(Player& p) {
             p.recordResult(m_win);
            if (m_win) {
                awardRewards(p);
            }
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
            
            cleanupGame(p);
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
                std::cout << "The deck is not valid!\n";
                return -1;
            }

            int temp = m_deck[index]; //save card before deleting it
            m_deck.erase(m_deck.begin() + index);
            return temp;
        }

        // i know its long,  refractor later
        // Ace, 2, 3, 4, 5, 6, 7, 8, 9, 10, J, Q, K.
        int cardValue(int rawCard) {
            switch (rawCard) {
                case 0:// choice of 1 or 11
                    return 11;// hardcode 11 for now
                case 1:
                    return 2; 
                case 2:
                    return 3;
                case 3:
                    return 4;
                case 4:
                    return 5;
                case 5:
                    return 6;
                case 6:
                    return 7;
                case 7:
                    return 8;
                case 8:
                    return 9;
                case 9:
                    return 10;
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
        bool m_draw {false}; //fix later
        // random         
        std::mt19937 m_rng{std::random_device{}()}; // engine
        std::bernoulli_distribution m_dist{0.5}; // 50/50, need to change it to pick from a random range from 0-12.
        Card m_deck {};
        int dealerTotal = 0;
        int dealerNumAces = 0;
        int playerTotal = 0;
        int playerNumAces = 0;
        
        enum class Outcome { PlayerBust, DealerBust, PlayerNatural, DealerNatural, Draw, PlayerWin, DealerWin };

    public:
        std::string getGameName() const {
            return "BlackJack";
        }

        void cleanupGame(Player& p) {
            dealerTotal = 0;
            dealerNumAces = 0;
            playerTotal = 0;
            playerNumAces = 0;
            p.recordResult(m_win);
            if (m_win) {
                awardRewards(p);
            }
            m_deck.resetDeck();
        }

        bool checkIfAceValue(int value, int& numAces) {
            if (value == 0) {
                ++numAces;
                return true;
            }
            return false;
        }

        void downgradeAce(int& total, int& numAces) {
            // if it went bust, it shuold downgrade.
            if (numAces >= 1) {
                if (total >= 22) {
                    total -= 10;
                    --numAces;
                }
            }
        }

        bool checkNaturalBlackJack(int v1, int v2) {
            return ((v1 + v2) == 21);
        }

        void resolveOutcome(Outcome result, Player& p) {
            switch(result) {
                case Outcome::DealerBust:
                    std::cout << "The Dealer went bust! You win!\n";
                    m_win = true;
                    break;
                case Outcome::DealerNatural:
                    std::cout << "The Dealer got blackjack in the first two cards. You lose.\n";
                    m_win = false;
                    break;
                case Outcome::DealerWin:
                    std::cout << "You lose.\n";
                    m_win = false;
                    break;
                case Outcome::Draw:
                    std::cout << "Its a draw.\n";
                    m_draw = true;
                    break;
                case Outcome::PlayerBust:
                    std::cout << "You went bust. You lose.\n";
                    m_win = false;
                    break;
                case Outcome::PlayerNatural:
                    std::cout << "You got blackjack in the first two cards. You win!\n";
                    m_win = true;
                    break;
                case Outcome::PlayerWin:
                    std::cout << "You win!\n";
                    m_win = true;
                    break;
            }
            cleanupGame(p);
        }

        void play(Player& p) {
            m_win = false;
            Outcome result;
            std::cout << "Welcome to BlackJack!\n";
            askBet(p);

            //rules.
            // dealer
            int firstDrawDealer = m_deck.drawCard();
            int secondDrawDealer = m_deck.drawCard();
                        
            dealerTotal += m_deck.cardValue(firstDrawDealer);
            dealerTotal += m_deck.cardValue(secondDrawDealer);

            // cant do or || since if both of them become ture, it would only incremenet 1, when it shuold incrememnt 2.
            if (checkIfAceValue(firstDrawDealer, dealerNumAces)) {
                downgradeAce(dealerTotal, dealerNumAces);
            } 
            
            if (checkIfAceValue(secondDrawDealer, dealerNumAces)) {
                downgradeAce(dealerTotal, dealerNumAces);
            }

            //player
            int firstDrawPlayer = m_deck.drawCard();
            int secondDrawPlayer = m_deck.drawCard();
            playerTotal += m_deck.cardValue(firstDrawPlayer);
            playerTotal += m_deck.cardValue(secondDrawPlayer);

            if (checkIfAceValue(firstDrawPlayer, playerNumAces)) {
                downgradeAce(playerTotal, playerNumAces);
            } 
            
            if (checkIfAceValue(secondDrawPlayer, playerNumAces)) {
                downgradeAce(playerTotal, playerNumAces);
            }

            if (dealerTotal == 21 && playerTotal == 21) {
                result = Outcome::Draw;
                resolveOutcome(result, p);
                return;
            } else if (dealerTotal == 21) {
                result = Outcome::DealerNatural;
                resolveOutcome(result, p);
                return;
            }

            std::cout << "The Dealer drew: " << m_deck.cardValue(firstDrawDealer) << ". The other card is a mystery.\n";
            std::cout << p.getName() << " drew: " << m_deck.cardValue(firstDrawPlayer) << " and " << m_deck.cardValue(secondDrawPlayer) << ".\n";

            if (playerTotal == 21) {
                result = Outcome::PlayerNatural;
                resolveOutcome(result, p);
                return;
            }

            while (playerTotal <= 21) {
                // hit or stand
                std::cout << "Your total is: " << playerTotal << ".\n";
                std::cout << "Would you like to Hit or Stand? ";
                char playerChoice;
                if (!(std::cin >> playerChoice)) {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    continue;
                }
                playerChoice = std::toupper(playerChoice);
                if (playerChoice == 'H') {
                    // draw card
                    int drawCardForPlayer = m_deck.drawCard();
                    playerTotal += m_deck.cardValue(drawCardForPlayer);
                    if (checkIfAceValue(drawCardForPlayer, playerNumAces)) {
                        downgradeAce(playerTotal, playerNumAces);
                    }

                    std::cout << "You chose Hit. " << p.getName() << " drew: " << m_deck.cardValue(drawCardForPlayer) << ". Your total is now: " << playerTotal << ".\n";
                    
                } else if (playerChoice == 'S') {
                    std::cout << "You chose stand.\n";
                    break;
                } else {
                    std::cout << "Not a valid choice. Try again.\n";
                }
            }

            if (playerTotal > 21) {
                result = Outcome::PlayerBust;
                resolveOutcome(result, p);
                return;
            }
            
            std::cout << "The other card Dealer drew was: " << m_deck.cardValue(secondDrawDealer) << ". The Dealer has: " << dealerTotal << ".\n";
            // dealer must pick up while 16 or below, and stand if 17 or above.
            while (dealerTotal < 17) {
                int drawCardForDealer = m_deck.drawCard();
                dealerTotal += m_deck.cardValue(drawCardForDealer);
                if (checkIfAceValue(drawCardForDealer, dealerNumAces)) {
                    downgradeAce(dealerTotal, dealerNumAces);
                }
                std::cout << "Dealer is drawing. Dealer drew: " << m_deck.cardValue(drawCardForDealer) << ". The Dealer's total is now: " << dealerTotal << ".\n";
            }

            std::cout << "The Dealer has: " << dealerTotal << ".\n";
            std::cout << "You have: " << playerTotal << ".\n";
            if (dealerTotal > 21) {
                result = Outcome::DealerBust;
            } else if (dealerTotal > playerTotal) {
                result = Outcome::DealerWin;
            } else if (dealerTotal < playerTotal) {
                result = Outcome::PlayerWin;
            } else {
                result = Outcome::Draw;
            }
            resolveOutcome(result, p);
            return;
        }
    };

int main() { 
    Player player {};
    std::cout << "Hello, " << player.getName() << "!\n";
    std::cout << "Your current balance is: " << player.getBalance() << '\n';
    std::cout << "Your current win rate is: " << player.getWinRatio() << "%\n";
    
    std::vector<std::unique_ptr<CasinoGame>> games;
    games.push_back(std::make_unique<CoinFlip>());
    games.push_back(std::make_unique<BlackJack>());
    
    
    while (true) {
        std::cout << "What would you like to play?\n";
        std::cout << "---------------------------------------\n";
        
        std::cout << "0: Quit\n";
        for (size_t i = 0; i < games.size(); ++i) {
            std::cout <<  i+1 << ": " << games[i]->getGameName() << '\n';
        }

        int selection {};
        if (!(std::cin >> selection)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }

        if (selection == 0) {
            std::cout << "Thanks for playing!\n";
            return 0;
        }

        if (selection < 1 || selection > static_cast<int>(games.size())) {
            std::cout << "Not a valid entry. Try again!\n";
            continue;
        }

        games[selection - 1]->play(player);
        std::cout << "Your current balance is: " << player.getBalance() << '\n';
    }

    return 0;
}