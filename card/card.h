// card.h
#pragma once

#include <iostream>
#include <random>
#include <string>
#include <vector>

struct PlayingCard {
    int rank; // 0-12
    int suit; // 0-3
};

inline std::ostream& operator<<(std::ostream& os, const PlayingCard& card) {
// std::ostream& operator<<(std::ostream& os, const PlayingCard& card) {
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
        int getRandomCard();

        PlayingCard drawCard();

        std::vector<PlayingCard> buildFullDeck();

        void resetDeck();
        
};