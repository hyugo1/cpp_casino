
#include "card.h"
#include "utils/utils.h"
#include <iostream>


int Card::getRandomCard() {
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

PlayingCard Card::drawCard() {
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

std::vector<PlayingCard> Card::buildFullDeck() {
    std::vector<PlayingCard> deck;
    for (int j {0}; j < 4; ++j) {
        for (int i {0}; i < 13; ++i) {
            deck.push_back({i, j});
        }
    }
    return deck;
}

void Card::resetDeck() {
    m_deck = buildFullDeck();
}