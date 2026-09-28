// game.cpp
#include <iostream>
#include <string>
#include <vector>

#include "game.h"
#include "utils/utils.h"
#include "player/player.h" 

void CasinoGame::askBet(Player& p) {
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

void CasinoGame::awardRewards(Player& p) {
    p.addToBalance(m_bet_amount * m_multiplier);
}

void CasinoGame::setMultiplier(double mul) {
    m_multiplier = mul;
}

void CasinoGame::resetMultiplier() {
    m_multiplier = 2;
}

void CasinoGame::refundBet(Player& p) {
    p.addToBalance(m_bet_amount);
}

void CasinoGame::resolveOutcome(CasinoGame::Outcome result, Player& p) {
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
