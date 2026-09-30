// blackjack.cpp
#include <iostream>
#include <string>
#include <vector>

#include "utils/utils.h"
#include "card/card.h"
#include "blackjack.h"
#include "game/game.h"
#include "player/player.h"

std::string BlackJack::getGameName() const  {
    return "BlackJack";
}

void BlackJack::cleanupGame() {
    m_deck.resetDeck();
}

// Ace, 2, 3, 4, 5, 6, 7, 8, 9, 10, J, Q, K.
int BlackJack::getCardValue(PlayingCard card) {
    if (card.rank == 0) {
        return 11;
    } else if (card.rank >= 10) {
        return 10;
    } else {
        return card.rank + 1;
    }
    return -1;
}

void BlackJack::addCard(int& total, int& numAces, PlayingCard card) {
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

    void BlackJack::setup(Player& p) {
    std::cout << "🎴 Welcome to BlackJack!\n";
    askBet(p);
}

void BlackJack::drawCards(RoundState& state) {
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

bool BlackJack::checkNatural(Player& p, RoundState& state) {
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

void BlackJack::printDrawnCards(Player& p, RoundState& state) {
        std::cout << "🎲 The dealer slides you your cards...\n";
        std::cout << "🂠 Dealer shows: " << state.firstDrawDealer 
                << " and a hidden card.\n\n";

        std::cout << "🃏 Your hand: " << state.firstDrawPlayer 
                << " and " << state.secondDrawPlayer << "\n";
        std::cout << "Your total: " << state.playerTotal << "\n\n";
    }

char BlackJack::askPlayerChoice() {
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

void BlackJack::drawAdditionalCardForPlayer(RoundState& state) {
    PlayingCard drawAdditionalCardForPlayer = m_deck.drawCard();
    addCard(state.playerTotal, state.playerNumAces, drawAdditionalCardForPlayer);

    std::cout << "You chose Hit. " << "You drew: " << drawAdditionalCardForPlayer << ". Your total is now: " << state.playerTotal << ".\n";
}

bool BlackJack::checkIfPlayerBust(Player& p, RoundState& state) {
    if (state.playerTotal > 21) {
        state.result = Outcome::PlayerBust;
        resolveOutcome(state.result, p);
        return true;
    }
    return false;
}

void BlackJack::dealerTurn(RoundState& state) {
    std::cout << "The other card Dealer drew was: " << state.secondDrawDealer << ". The Dealer has: " << state.dealerTotal << ".\n";
    while (state.dealerTotal < 17) {
        PlayingCard drawCardForDealer = m_deck.drawCard();
        addCard(state.dealerTotal, state.dealerNumAces, drawCardForDealer);

        std::cout << "Dealer is drawing. Dealer drew: " << drawCardForDealer << ". The Dealer's total is now: " << state.dealerTotal << ".\n";
    }
    std::cout << "✋ Dealer stands.\n\n";
}

void BlackJack::checkOutcome(RoundState& state) {
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

void BlackJack::playerTurn(RoundState& state) {
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

bool BlackJack::isBlackjack(Player& p, RoundState& state) {
    if (state.playerTotal == 21) {
        state.result = Outcome::PlayerNatural;
        resolveOutcome(state.result, p);
        return true;
    }
    return false;
}

void BlackJack::play(Player& p) {
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