#include <iostream>
#include <string>
#include <limits>
#include <cctype>
#include <random>
#include <vector>
#include <memory>
#include <array>

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
        double m_bal {100};
        int m_total_games_played {0};
        int m_wins {0};
        int m_losses {0};
        std::vector<std::string> difficultyLevel {"easy", "medium", "hard"};
        int m_difficulty {4};

    public:
        Player(std::string name="Guest", double bal=100.0, int games_played=0, int wins=0) : m_name{name}, m_bal {bal}, m_total_games_played {games_played}, m_wins {wins} {};

        const std::string_view getName() const { return m_name; }
        void setName(const std::string& name) {m_name = name;}
        double getBalance() const { return m_bal; }
        double getWinRatio() const { 
            if (m_total_games_played == 0) return 0.0;
            return (static_cast<double>(m_wins) / m_total_games_played) * 100;
        }

        void addToBalance(double amount) { m_bal += amount; }
        void subFromBalance(double amount) { m_bal -= amount; }
        void recordResult(bool won) {
            if (won) { ++m_wins; }
            else { ++m_losses; }
            ++m_total_games_played;
        }

        void askDifficulty() {
            int choice {};
            do {
                std::cout << "What difficulty would you like?\n";
                std::cout << "1: Easy 2: Medium or 3: Hard? Enter a number: ";
                if (!Utils::tryRead(choice)) {continue; }
            } while (choice < 1 || choice > 3);
            m_difficulty = choice;
            std::cout << "Chose: " << difficultyLevel[m_difficulty - 1] << ".\n";
        }
        
        int getDifficulty() const {
            return m_difficulty;
        }
};

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
            p.addToBalance(m_bet_amount * m_multiplier);
        }

        void setMultiplier(double mul) {
            m_multiplier = mul;
        }
        
        void resetMultiplier() {
            m_multiplier = 2;
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

            std::cout << "🎴 Welcome to Heads or Tails!\n";
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
                std::cout << "❌ Invalid input. Please enter H or T.\n";
            }

            if (guess == bot_decision) {
                resolveOutcome(Outcome::PlayerWin, p);
            } else {
                resolveOutcome(Outcome::PlayerLose, p);
            }
        }
};

// enum class Suit { Spades, Hearts, Diamonds, Clubs };
// enum class Rank { Ace, Two, Three, Four, Five, Six, Seven, Eight, Nine, Ten, Jack, Queen, King };
// struct PlayingCard {
//     Rank rank;
//     Suit suit;
// };

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
        os << "❌ Invalid Card";
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
        std::string getGameName() const override {
            return "GuessTheCard";
        }

        void cleanupGame() override {
            resetMultiplier();
            m_deck.resetDeck();
        }

        void setNumOfLives(int lives) {
            m_numOflives = lives;
        }

        void checkRankGuess(int rankGuess, int cardRank) {
            if (rankGuess > cardRank) {
                std::cout << "Your rank guess is too high. Try again!\n";
            } else if (rankGuess < cardRank) {
                std::cout << "Your rank guess is too low. Try again!\n";
            } else {
                std::cout << "You guessed the correct rank! ";
            }
        }

        int moveByOne(int rankGuess) {
            return rankGuess + 1;
        }

        int getRankGuess() {
            int rank;
            while (true) {
                std::cout << "Enter rank (Ace=1, Jack=11, Queen=12, King=13): ";
                if (!Utils::tryRead(rank)) {
                    continue;
                }
                if (rank >= 1 && rank <= 13) {
                    return rank - 1; // convert to 0–12
                }
                std::cout << "❌ Invalid rank. Try again.\n";
            }
        }

        std::string getSuitGuess() {
            std::string suit;
            while (true) {
                std::cout << "What suit is the card(spades, hearts, diamonds, clubs)? Type your guess: ";
                if (!Utils::tryRead(suit)) {
                    continue;
                } 
                for (char& c : suit) {
                    c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
                }

                for (const auto& s : suits) {
                    if (suit == s) {
                        return suit;
                    }
                }
                std::cout << "❌ Invalid suit. Try again.\n";
            }
        }

        void runEasyMode(RoundState& state) {
            if (!state.hasGuessedRank) {
                int guess = getRankGuess();
                checkRankGuess(guess, card.rank);
                state.tries++;

                if (guess == card.rank) {
                    state.guessedCorrectly = true;
                    state.hasGuessedRank = true;
                    std::cout << "Correct!\n";
                    return;
                }
            }
        }

        void runStandardMode(RoundState& state) {
            int activeRank = state.hasGuessedRank ? state.confirmedRank : getRankGuess();
            std::string activeSuit = state.hasGuessedSuit ? state.confirmedSuit : getSuitGuess();

            std::cout << "You guessed: " << moveByOne(activeRank) << " of " << activeSuit << "\n";

            if (activeSuit == suits[card.suit] && activeRank == card.rank) {
                ++state.tries; 
                state.guessedCorrectly = true;
                std::cout << "You got the correct suit and rank!\n";
            } else if (activeSuit == suits[card.suit]) {
                ++state.tries; 
                state.hasGuessedSuit = true;
                state.confirmedSuit = activeSuit;
                std::cout << "You got only the suit correct.\n";
                checkRankGuess(activeRank, card.rank);
            } else if (activeRank == card.rank) {
                ++state.tries; 
                state.hasGuessedRank = true;
                state.confirmedRank = activeRank;
                std::cout << "You got only the rank correct.\n";
            } else {
                ++state.tries; 
                std::cout << "You got both wrong.\n";
                checkRankGuess(activeRank, card.rank);
            }
        }

        void setup(Player& p) override {
            std::cout << "🎴 Welcome to 'Guess The Card'!\n\n";

            askBet(p);
            card = m_deck.drawCard();

            p.askDifficulty();

            std::cout << "\nThe deck has been shuffled, and a card has been drawn...\n";

            if (p.getDifficulty() == 1) {
                m_numOflives = 10;
                std::cout << "🟢 Easy Mode Selected!\n";
                std::cout << "Your challenge: Guess the *rank* of the card (e.g., Ace, 7, King).\n";
                std::cout << "You have " << m_numOflives << " lives. Good luck!\n";
            } 
            else if (p.getDifficulty() == 2) {
                m_numOflives = 10;
                std::cout << "🟡 Medium Mode Selected!\n";
                std::cout << "Your challenge: Guess the *exact card* (rank + suit).\n";
                std::cout << "You have " << m_numOflives << " lives. Stay sharp!\n";
            } 
            else {
                m_numOflives = 6;
                std::cout << "🔴 Hard Mode Selected!\n";
                std::cout << "Your challenge: Guess the *exact card* (rank + suit).\n";
                std::cout << "You only have " << m_numOflives << " lives... choose wisely.\n";
            }

            std::cout << "\nLet the game begin!\n\n";
        }

        void showAnswer(RoundState& state) {
            std::cout << "The card was: " << card << "\n";
            if (state.guessedCorrectly) {
                std::cout << "It took " << state.tries;
                if (state.tries == 1) {
                    std::cout << " try.\n";
                } else {
                    std::cout << " tries.\n"; 
                }
            }
        }

        void play(Player& p) override {
            setup(p);

            RoundState state;

            while (state.tries < m_numOflives && !state.guessedCorrectly) {
                std::cout << "Lives left: " << m_numOflives - state.tries << "\n";
                if (p.getDifficulty() == 1) {
                    runEasyMode(state);
                } else {
                    runStandardMode(state);
                }
            }
            double maxMultiplier = (p.getDifficulty() == 1) ? 4.0 : (p.getDifficulty() == 2) ? 6.0 : 8.0;
            double minMultiplier = 1.5;
            double multiplier = maxMultiplier;
            if (m_numOflives > 1) {
                multiplier = maxMultiplier - ((maxMultiplier - minMultiplier) * (state.tries - 1) / (m_numOflives - 1));
            }

            setMultiplier(multiplier);
            
            Outcome result = state.guessedCorrectly ? Outcome::PlayerWin : Outcome::PlayerLose;

            showAnswer(state);
            if (!state.guessedCorrectly) {
                std::cout << "You ran out of lives. You lose.\n"; 
            }
            resolveOutcome(result, p);
        }
};

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
        std::string getGameName() const override {
            return "BlackJack";
        }

        void cleanupGame() override {
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

         void setup(Player& p) override {
            std::cout << "🎴 Welcome to BlackJack!\n";
            askBet(p);
        }

        void drawCards(RoundState& state) {
            // dealer
            state.firstDrawDealer = m_deck.drawCard();
            state.secondDrawDealer = m_deck.drawCard();
            
            addCard(state.dealerTotal, state.dealerNumAces, state.firstDrawDealer);
            addCard(state.dealerTotal, state.dealerNumAces, state.secondDrawDealer);

            //player
            state.firstDrawPlayer = m_deck.drawCard();
            state.secondDrawPlayer = m_deck.drawCard();

            addCard(state.playerTotal, state.playerNumAces, state.firstDrawPlayer);
            addCard(state.playerTotal, state.playerNumAces, state.secondDrawPlayer);
        }

        bool checkNatural(Player& p, RoundState& state) {
            if (state.dealerTotal == 21 && state.playerTotal == 21) {
                state.result = Outcome::Draw;
                resolveOutcome(state.result, p);
                return true;
            } else if (state.dealerTotal == 21) {
                state.result = Outcome::DealerNatural;
                resolveOutcome(state.result, p);
                return true;
            } 

            return false;
        }

        void printDrawnCards(Player& p, RoundState& state) {
                std::cout << "🎲 The dealer slides you your cards...\n";
                std::cout << "🂠 Dealer shows: " << state.firstDrawDealer 
                        << " and a hidden card.\n\n";

                std::cout << "🃏 Your hand: " << state.firstDrawPlayer 
                        << " and " << state.secondDrawPlayer << "\n";
                std::cout << "Your total: " << state.playerTotal << "\n\n";
            }

        char askPlayerChoice() {
            char c;
            while (true) {
                std::cout << "Would you like to Hit or Stand? ";
                if (!Utils::tryRead(c)) {
                    continue;
                }
                c = std::toupper(c);
                if (c == 'H' || c == 'S') {
                    return c;
                }
                std::cout << "❌ Invalid character. Try again.\n";
            }
        }

        void drawAdditionalCardForPlayer(RoundState& state) {
            PlayingCard drawAdditionalCardForPlayer = m_deck.drawCard();
            addCard(state.playerTotal, state.playerNumAces, drawAdditionalCardForPlayer);

            std::cout << "You chose Hit. " << "You drew: " << drawAdditionalCardForPlayer << ". Your total is now: " << state.playerTotal << ".\n";
        }

        bool checkIfPlayerBust(Player& p, RoundState& state) {
            if (state.playerTotal > 21) {
                state.result = Outcome::PlayerBust;
                resolveOutcome(state.result, p);
                return true;
            }
            return false;
        }

        void dealerTurn(RoundState& state) {
            std::cout << "The other card Dealer drew was: " << state.secondDrawDealer << ". The Dealer has: " << state.dealerTotal << ".\n";
            while (state.dealerTotal < 17) {
                PlayingCard drawCardForDealer = m_deck.drawCard();
                addCard(state.dealerTotal, state.dealerNumAces, drawCardForDealer);

                std::cout << "Dealer is drawing. Dealer drew: " << drawCardForDealer << ". The Dealer's total is now: " << state.dealerTotal << ".\n";
            }
            std::cout << "✋ Dealer stands.\n\n";
        }

        void checkOutcome(RoundState& state) {
            std::cout << "The Dealer has: " << state.dealerTotal << ".\n";
            std::cout << "You have: " << state.playerTotal << ".\n";
            if (state.dealerTotal > 21) {
                state.result = Outcome::DealerBust;
            } else if (state.dealerTotal > state.playerTotal) {
                state.result = Outcome::PlayerLose;
            } else if (state.dealerTotal < state.playerTotal) {
                state.result = Outcome::PlayerWin;
            } else {
                state.result = Outcome::Draw;
            }
        }

        void playerTurn(RoundState& state) {
            // hit or stand
            while (state.playerTotal < 21) {
                std::cout << "Your total is: " << state.playerTotal << ".\n";
                char c = askPlayerChoice();
                if (c == 'H') {
                    // draw card
                    drawAdditionalCardForPlayer(state);
                } else if (c == 'S') {
                    std::cout << "✋ You chose stand.\n";
                    break;
                } 
            }
        }

        bool isBlackjack(Player& p, RoundState& state) {
            if (state.playerTotal == 21) {
                state.result = Outcome::PlayerNatural;
                resolveOutcome(state.result, p);
                return true;
            }
            return false;
        }

        void play(Player& p) override {
            setup(p);
            RoundState state;

            drawCards(state);
            
            printDrawnCards(p, state);
            
            if (checkNatural(p, state)) {
                return;
            }
            
            if (isBlackjack(p, state)) {
                return;
            }

            playerTurn(state);

            if (checkIfPlayerBust(p, state)) {
                return;
            }

            dealerTurn(state); // dealer must pick up while 16 or below, and stand if 17 or above.

            checkOutcome(state);
            resolveOutcome(state.result, p);
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
    games.push_back(std::make_unique<GuessTheCard>());
    
    
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
        std::cout << "Your current win rate is: " << player.getWinRatio() << "%\n";
        std::cout << '\n';
    }

    return 0;
}