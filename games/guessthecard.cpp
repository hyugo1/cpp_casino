// guessthecard.cpp
#include <iostream>
#include <string>
#include <vector>

#include "utils/utils.h"
#include "card/card.h"
#include "guessthecard.h"
#include "game/game.h"
#include "player/player.h"

// enum class Suit { Spades, Hearts, Diamonds, Clubs };
// enum class Rank { Ace, Two, Three, Four, Five, Six, Seven, Eight, Nine, Ten, Jack, Queen, King };
// struct PlayingCard {
//     Rank rank;
//     Suit suit;
// };

std::string GuessTheCard::getGameName() const {
    return "GuessTheCard";
}

void GuessTheCard::cleanupGame() {
    resetMultiplier();
    m_deck.resetDeck();
}

void GuessTheCard::setNumOfLives(int lives) {
    m_numOflives = lives;
}

void GuessTheCard::checkRankGuess(int rankGuess, int cardRank) {
    if (rankGuess > cardRank) {
        std::cout << "Your rank guess is too high. Try again!\n";
    } else if (rankGuess < cardRank) {
        std::cout << "Your rank guess is too low. Try again!\n";
    } else {
        std::cout << "You guessed the correct rank! ";
    }
}

int GuessTheCard::moveByOne(int rankGuess) {
    return rankGuess + 1;
}

int GuessTheCard::getRankGuess() {
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

std::string GuessTheCard::getSuitGuess() {
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

void GuessTheCard::runEasyMode(RoundState& state) {
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

void GuessTheCard::runStandardMode(RoundState& state) {
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

void GuessTheCard::setup(Player& p) {
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

void GuessTheCard::showAnswer(RoundState& state) {
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

void GuessTheCard::play(Player& p) {
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
