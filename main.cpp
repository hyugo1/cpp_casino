#include <iostream>
#include <string>
#include <limits>
#include <cctype>
#include <random>
#include <vector>
#include <memory>

namespace Utils {
    template <typename T>
    bool tryRead(T& value) {
        if (!(std::cin >> value)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return false;
        }
        return true;
    }
}

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
        void setName(const std::string& name) {m_name = name;}
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
        virtual void cleanupGame() {};
        virtual std::string getGameName() const = 0;
        virtual ~CasinoGame() = default;
        
        enum class Outcome { PlayerBust, DealerBust, PlayerNatural, DealerNatural, Draw, PlayerWin, PlayerLose };
        
    private:
        bool m_win {false};
        bool m_draw {false};
        int m_bet_amount {0};

    protected:
        void askBet(Player& p) {
            do {
                std::cout << "How much are you betting? "; 
                if (!Utils::tryRead(m_bet_amount)) {continue; }
                if (m_bet_amount > p.getBalance()) {
                    std::cout << "You can't afford that. Try again\n";
                }
            } while (m_bet_amount > p.getBalance() || m_bet_amount <= 0);
            std::cout << "Betted: " << m_bet_amount << '\n';
            p.subFromBalance(m_bet_amount);
        }

        void awardRewards(Player& p) {
            p.addToBalance(m_bet_amount * 2);
        }

        void refundBet(Player& p) {
            p.addToBalance(m_bet_amount);
        }

        void resolveOutcome(Outcome result, Player& p) {
            m_win = false;
            m_draw = false;
            switch(result) {
                case Outcome::DealerBust:
                    std::cout << "The Dealer went bust! You win!\n";
                    m_win = true;
                    break;
                case Outcome::DealerNatural:
                    std::cout << "The Dealer got blackjack in the first two cards. You lose.\n";
                    m_win = false;
                    break;
                case Outcome::PlayerLose:
                    std::cout << "You lose.\n";
                    m_win = false;
                    break;
                case Outcome::Draw:
                    std::cout << "Its a draw.\n";
                    m_win = false;
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
            if (!m_draw) {
                p.recordResult(m_win);
            }
            if (m_win) {
                awardRewards(p);
            } else if (m_draw) {
                refundBet(p);
            }

            cleanupGame();
        }
};

class CoinFlip : public CasinoGame {
    private:
        std::mt19937 m_rng{std::random_device{}()}; // engine
        std::bernoulli_distribution m_dist{0.5};// 50/50
        
    public:
        std::string getGameName() const override {
            return "CoinFlip";
        }

        void cleanupGame() override {
        }

        void play(Player& p) override {
            bool coin = m_dist(m_rng); // true or false
            char bot_decision = coin ? 'H' : 'T';

            std::cout << "Welcome to Heads or Tails!\n";
            askBet(p);
            
            char guess;
            while (true) {
                std::cout << "Heads or Tails? Type H or T.\n";
                if (!Utils::tryRead(guess)) {continue;}

                guess = std::toupper(guess);
                if (guess == 'T' || guess == 'H') {
                    // try again???
                    break;
                }
                std::cout << "Invalid input. Please enter H or T.\n";
            }

            if (guess == bot_decision) {
                resolveOutcome(Outcome::PlayerWin, p);
            } else {
                resolveOutcome(Outcome::PlayerLose, p);
            }
        }
};


struct PlayingCard {
    int rank; // 0-12
    int suit; // 0-3
};

std::ostream& operator<<(std::ostream& os, const PlayingCard& card) {
    static const std::string ranks[] {
        "Ace", "2", "3", "4", "5", "6", "7",
        "8", "9", "10", "Jack", "Queen", "King"
    };

    static const std::string suits[] {
        "Spades", "Hearts", "Diamonds", "Clubs"
    };

    if (card.rank < 0 || card.rank > 12 ||
        card.suit < 0 || card.suit > 3) {
        os << "Invalid Card";
        return os;
    }

    os << ranks[card.rank] << " of " << suits[card.suit];
    return os;
}
class Card {
    private:
        // rank: 0-12, suit: 0-3
        std::vector<PlayingCard> m_deck {  buildFullDeck() };

        // jack, queen, king is all 10, Ace = 1 or 11 in blackjack.
        std::random_device rd;
        std::mt19937 gen{ rd() };
        const int exitNumber = 999;

    public:
        int getRandomCard() {
            // 1. Define the distribution range based on the vector's indices
            // uniform_int_distribution is inclusive: [0, numbers.size() - 1]
            if (m_deck.size() <= 0) {
                return exitNumber;
            }
            std::uniform_int_distribution<size_t> distr(0, m_deck.size() - 1);

            // 2. Generate a random index
            int random_index = distr(gen);
            
            //return shuold be the index, which is the card number
            return random_index;
        }

        PlayingCard drawCard() {
            int index = getRandomCard();
            if (index == exitNumber) {
                std::cout << "The deck is not valid!\n";
                return {-1, -1};
            }

            int ranks = m_deck[index].rank; //save card before deleting it
            int suits = m_deck[index].suit; //save card suit before deleting it
            m_deck.erase(m_deck.begin() + index);
            return {ranks, suits};
        }

        std::vector<PlayingCard> buildFullDeck() {
            std::vector<PlayingCard> deck;
            for (int j {0}; j < 4; ++j) {
                for (int i {0}; i < 13; ++i) {
                    deck.push_back({i, j});
                }
            }
            return deck;
        }

        void resetDeck() {
            m_deck = buildFullDeck();
        }
};

class BlackJack : public CasinoGame {
    private:
        Card m_deck {};
        int dealerTotal = 0;
        int dealerNumAces = 0;
        int playerTotal = 0;
        int playerNumAces = 0;

    public:
        std::string getGameName() const override {
            return "BlackJack";
        }

        void cleanupGame() override {
            dealerTotal = 0;
            dealerNumAces = 0;
            playerTotal = 0;
            playerNumAces = 0;
            m_deck.resetDeck();
        }

        // Ace, 2, 3, 4, 5, 6, 7, 8, 9, 10, J, Q, K.
        int getCardValue(PlayingCard card) {
            if (card.rank == 0) {
                return 11;
            } else if (card.rank >= 10) {
                return 10;
            } else {
                return card.rank + 1;
            }
            return -1;
        }

        void addCard(int& total, int& numAces, PlayingCard card) {
            total += getCardValue(card);

            if (card.rank == 0) {
                ++numAces;
            }

            //downgrade ace
            while (total > 21 && numAces > 0) {
                total -= 10;
                --numAces;
            }
        }

        void play(Player& p) override {
            Outcome result;
            std::cout << "Welcome to BlackJack!\n";
            askBet(p);

            //rules.
            // dealer
            PlayingCard firstDrawDealer = m_deck.drawCard();
            PlayingCard secondDrawDealer = m_deck.drawCard();
            
            addCard(dealerTotal, dealerNumAces, firstDrawDealer);
            addCard(dealerTotal, dealerNumAces, secondDrawDealer);

            //player
            PlayingCard firstDrawPlayer = m_deck.drawCard();
            PlayingCard secondDrawPlayer = m_deck.drawCard();

            addCard(playerTotal, playerNumAces, firstDrawPlayer);
            addCard(playerTotal, playerNumAces, secondDrawPlayer);

            if (dealerTotal == 21 && playerTotal == 21) {
                result = Outcome::Draw;
                resolveOutcome(result, p);
                return;
            } else if (dealerTotal == 21) {
                result = Outcome::DealerNatural;
                resolveOutcome(result, p);
                return;
            }

            std::cout << "The Dealer drew: " << firstDrawDealer  << ". The other card is a mystery.\n";
            std::cout << p.getName() << " drew: " << firstDrawPlayer << " and " << secondDrawPlayer << ".\n";

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
                if (!Utils::tryRead(playerChoice)) {continue;}
                playerChoice = std::toupper(playerChoice);
                if (playerChoice == 'H') {
                    // draw card
                    PlayingCard drawCardForPlayer = m_deck.drawCard();
                    addCard(playerTotal, playerNumAces, drawCardForPlayer);

                    std::cout << "You chose Hit. " << p.getName() << " drew: " << drawCardForPlayer << ". Your total is now: " << playerTotal << ".\n";
                    
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
            
            std::cout << "The other card Dealer drew was: " << secondDrawDealer << ". The Dealer has: " << dealerTotal << ".\n";
            // dealer must pick up while 16 or below, and stand if 17 or above.
            while (dealerTotal < 17) {
                PlayingCard drawCardForDealer = m_deck.drawCard();
                addCard(dealerTotal, dealerNumAces, drawCardForDealer);

                std::cout << "Dealer is drawing. Dealer drew: " << drawCardForDealer << ". The Dealer's total is now: " << dealerTotal << ".\n";
            }

            std::cout << "The Dealer has: " << dealerTotal << ".\n";
            std::cout << "You have: " << playerTotal << ".\n";
            if (dealerTotal > 21) {
                result = Outcome::DealerBust;
            } else if (dealerTotal > playerTotal) {
                result = Outcome::PlayerLose;
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
    Player player{};
    
    std::string name;
    std::cout << "Enter your name: ";
    std::getline(std::cin, name);
    player.setName(name);

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
        if (!Utils::tryRead(selection)) { continue; }

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